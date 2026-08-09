#include "bluetooth.h"
#include "main.h"
#include "pid.h"        /* 用 PID_t / PID_SetGains */
#include <stdio.h>
#include <string.h>
#include "control.h"

extern UART_HandleTypeDef huart1;

#define BT_TX_TIMEOUT   100

/* ===================== 发送部分 ===================== */

/* printf 重定向（可选，不想用 printf 可以删） */
int _write(int file, char *ptr, int len)
{
    /* IT 非阻塞版（和你发送函数一致，不卡主循环） */
    if (huart1.gState == HAL_UART_STATE_READY)
        HAL_UART_Transmit_IT(&huart1, (uint8_t *)ptr, len);
    return len;
}

static void BT_SendRaw(const uint8_t *data, uint16_t len)
{
    if (huart1.gState != HAL_UART_STATE_READY)
        return;
    HAL_UART_Transmit_IT(&huart1, (uint8_t *)data, len);
}

void BT_SendString(const char *str)
{
    /* 注意：IT 模式下不能"分两次发"，一次拼好一整行再发 */
    char buf[80];
    int n = snprintf(buf, sizeof(buf), "%s\r\n", str ? str : "");
    if (n > 0) BT_SendRaw((const uint8_t *)buf, (uint16_t)n);
}

void BT_SendPose(int16_t pitch_filt, int16_t roll_filt,
                 int16_t pitch_raw,  int16_t roll_raw)
{
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "%d,%d,%d,%d\r\n",
                     (int)pitch_filt, (int)roll_filt,
                     (int)pitch_raw,  (int)roll_raw);
    if (n > 0) BT_SendRaw((const uint8_t *)buf, (uint16_t)n);
}

/* ===================== 接收部分 ===================== */

volatile uint8_t bt_rx_byte;      /* 单字节接收缓冲 */
char bt_rx_line[64];              /* 行缓冲：攒成一行的命令 */
volatile uint8_t bt_rx_index = 0; /* 当前攒到第几个字节 */

/* 开启接收：初始化后调用一次 */
void BT_RxStart(void)
{
    HAL_UART_Receive_IT(&huart1, &bt_rx_byte, 1);
}

/* UART 接收完成回调：收到一字节就进来一次 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* 收到换行符 或 缓冲快满 → 攒完一行，去解析 */
        if (bt_rx_byte == '\n' || bt_rx_index >= sizeof(bt_rx_line) - 1)
        {
            bt_rx_line[bt_rx_index] = '\0';   /* 补字符串结束符 */
            BT_ParseLine(bt_rx_line);         /* 解析这一行 */
            bt_rx_index = 0;                  /* 重置行缓冲 */
        }
        else if (bt_rx_byte != '\r')          /* 丢弃回车符，其余入行缓冲 */
        {
            bt_rx_line[bt_rx_index++] = (char)bt_rx_byte;
        }

        /* ★关键：重新武装接收，否则只收一字节就停了★ */
        HAL_UART_Receive_IT(&huart1, &bt_rx_byte, 1);
    }
}

/* 解析命令：手机发送  "kp=2500,kd=80"  →  Kp=25.00, Kd=0.80 */
void BT_ParseLine(char *line)
{

    int kp_x, kd_x, skp_x, ski_x, tr_x;
    /* ① 直立环：kp=9900,kd=50 */
    if (sscanf(line, "kp=%d,kd=%d", &kp_x, &kd_x) == 2)
    {
        PID_SetGains(&balance_pid, kp_x/100.0f, 0.0f, kd_x/100.0f);
        char ack[64];
        int n = sprintf(ack, "KP=%.2f KD=%.2f OK\r\n",
                        balance_pid.kp, balance_pid.kd);
        HAL_UART_Transmit_IT(&huart1, (uint8_t*)ack, n);
    }
    /* ② 速度环：skp=60,ski=3  →  Kp=0.60, Ki=0.03 */
    else if (sscanf(line, "skp=%f,ski=%f", &skp_x, &ski_x) == 2)
    {
        PID_SetGains(&speed_pid, skp_x/100.0f, ski_x/100.0f, 0.0f);
        char ack[64];
        int n = sprintf(ack, "SKP=%.2f SKI=%.3f OK\r\n",
                        speed_pid.kp, speed_pid.ki);
        HAL_UART_Transmit_IT(&huart1, (uint8_t*)ack, n);
    }
    /* ③ 目标角：tr=130  →  1.30° */
    else if (sscanf(line, "tr=%d", &tr_x) == 1)
    {
        angle_target = tr_x / 100.0f;
        char ack[64];
        int n = sprintf(ack, "TR=%.2f OK\r\n", (double)angle_target);
        HAL_UART_Transmit_IT(&huart1, (uint8_t*)ack, n);
    }
}
