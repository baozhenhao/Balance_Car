#include "control.h"
#include "mpu6050.h"
#include "encoder.h"
#include "motor.h"

/* ===== 两环实例 + 参数 ===== */
PID_t balance_pid;              /* 直立环 */
PID_t speed_pid;                /* 速度环 */
float angle_target = -0.6f;     /* 机械中值(tr)，可蓝牙改 */

/* 内部状态 */
static float angle_cmd     = 0.0f;   /* 速度环输出：角度修正 */
static float balance_out   = 0.0f;   /* 直立环输出 */
static float speed_filtered = 0.0f;  /* 速度低通滤波 */

void Control_Init(void)
{
    /* 直立环：PD */
    PID_Init(&balance_pid, 99.0f, 0.1f, 0.5f, -800.0f, 800.0f);
    balance_pid.use_integral = 0;

    /* 速度环：PI */
    PID_Init(&speed_pid, 0.003f, 0.005f, 0.0f, -5.0f, 5.0f);
}

/* ===== 速度环：50ms 一次 =====
   编码器速度 → 低通滤波 → PI → 角度修正 angle_cmd */
void Control_SpeedLoop(float dt)
{
    Encoder_Update();   /* 读编码器（50ms 清一次） */

    float avg_speed = -(enc_left_speed + enc_right_speed) * 0.5f;

    /* 一阶低通：让速度变化平缓，防过冲振荡 [4]
       50ms 更新率下系数别太大，响应慢了就调小平滑 */
    speed_filtered = 0.5f * speed_filtered + 0.5f * avg_speed;

    angle_cmd = PID_Update(&speed_pid, 0.0f, speed_filtered, dt);
    if (angle_cmd >  5.0f) angle_cmd =  5.0f;   /* 限幅±5° */
    if (angle_cmd < -5.0f) angle_cmd = -5.0f;
}

/* ===== 直立环：5ms 一次 =====
   读MPU + 互补滤波 → PD → 电机（目标角 = 中值 + 速度环修正） */
void Control_BalanceLoop(float dt)
{
    MPU6050_GetAngle(dt);   /* 读MPU + 滤波（和直立环同周期） */

    balance_out = PID_Update(&balance_pid,
                             angle_target + angle_cmd,
                             MPU_Pitch, dt);
    Motor_SetSpeed(MOTOR_LEFT,  (int16_t)balance_out);
    Motor_SetSpeed(MOTOR_RIGHT, (int16_t)balance_out);
}

/* ===== 供 main 打印 ===== */
float Control_GetBalanceOut(void) { return balance_out; }
float Control_GetAngleCmd(void)   { return angle_cmd;   }

