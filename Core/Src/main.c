/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "motor.h"
#include "encoder.h"
#include "mpu6050.h"
#include "string.h"
#include "control.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern float MPU_Pitch;
extern float MPU_Roll;
extern float MPU_Gyro_Y;

#define MECHANICAL_BALANCE_CENTER  2.4f
#define PWM_MAX_LIMIT             1000

// 引入我们的控制参数和速度变量（来自 control.c 和 encoder.c）
extern int16_t enc_left_speed;
extern int16_t enc_right_speed;
//extern float MPU_Pitch;
//extern float MPU_Roll;
//extern float MPU_Gyro_Y;
//
//#define MECHANICAL_BALANCE_CENTER  6.9f
//#define PWM_MAX_LIMIT             1000
//
//float Vertical_Kp = 35.0f;
//float Vertical_Kd = 0.15f;
//
//int32_t Vertical_Loop(float current_angle, float gyro_speed)
//{
//    float angle_error;
//    int32_t pwm_output;
//
//    angle_error = current_angle - MECHANICAL_BALANCE_CENTER;
//    pwm_output = (int32_t)(Vertical_Kp * angle_error + Vertical_Kd * gyro_speed);
//
//    if (pwm_output > PWM_MAX_LIMIT)   pwm_output = PWM_MAX_LIMIT;
//    if (pwm_output < -PWM_MAX_LIMIT)  pwm_output = -PWM_MAX_LIMIT;
//
//    return pwm_output;
//}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  // 1. 点亮小蓝板绿色指示灯
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    // 2. 依次唤醒三大器官 (顺序不能乱)
   MPU6050_Init();    // 唤醒小脑
   Encoder_Init();    // 唤醒测速神经
   Motor_Init();      // 唤醒肌肉 (初始速度为0)
   // =================== 软件 I2C 强行唤醒 MPU6050 开始 ===================
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);

    uint32_t d;
    // [Start]
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);   d=30; while(d--);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); d=30; while(d--);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=30; while(d--);
    // [Send 0xD0]
    uint8_t val1 = 0xD0;
    for(int i=0; i<8; i++) {
        if(val1 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
        val1 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
    // [Send 0x6B]
    uint8_t val2 = 0x6B;
    for(int i=0; i<8; i++) {
        if(val2 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
        val2 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
    // [Send 0x00]
    uint8_t val3 = 0x00;
    for(int i=0; i<8; i++) {
        if(val3 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
        val3 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
    // [Stop]
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=30; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); d=30; while(d--);
    // =================== 软件 I2C 强行唤醒 MPU6050 结束 ===================

    HAL_Delay(100);

    // 3. 开启定时器中断（用来跑闭环控制 Control_Loop，假设用的是定时器 TIM1）
    HAL_TIM_Base_Start_IT(&htim1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   /* USER CODE BEGIN WHILE */
   // 1. GPIO 初始化
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//   __HAL_RCC_GPIOA_CLK_ENABLE();
//
//   GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
//   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//   GPIO_InitStruct.Pull = GPIO_PULLUP;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
//
//   // 2. 软件 I2C 唤醒 MPU6050 (往 0x6B 写入 0x00)
//   uint32_t d;
//   // [Start]
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);   d=30; while(d--);
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); d=30; while(d--);
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=30; while(d--);
//   // [Send 0xD0]
//   uint8_t val1 = 0xD0;
//   for(int i=0; i<8; i++) {
//       if(val1 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//       val1 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//   }
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//   // [Send 0x6B]
//   uint8_t val2 = 0x6B;
//   for(int i=0; i<8; i++) {
//       if(val2 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//       val2 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//   }
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//   // [Send 0x00]
//   uint8_t val3 = 0x00;
//   for(int i=0; i<8; i++) {
//       if(val3 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//       val3 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//   }
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//   // [Stop]
//   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=30; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); d=30; while(d--);
//
//   HAL_Delay(100);
uint32_t test_count = 0;



	     while (1)
	     {
	         test_count++;

	         // 1. 纯粹用于蓝牙打印监视（50ms 打印一次即可）
	         uint8_t pin_status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
	         char txt_buf[128];
	         int32_t p_show = (int32_t)(MPU_Pitch * 100.0f);
	         int32_t r_show = (int32_t)(MPU_Roll * 100.0f);

	         sprintf(txt_buf, "Cnt:%ld | PIN12:%d | P:%ld.%02ld R:%ld.%02ld | L:%d R:%d\r\n",
	                 test_count,
	                 pin_status,
	                 p_show / 100, (p_show < 0 ? -p_show : p_show) % 100,
	                 r_show / 100, (r_show < 0 ? -r_show : r_show) % 100,
	                 enc_left_speed, enc_right_speed);

	         HAL_UART_Transmit(&huart1, (uint8_t *)txt_buf, strlen(txt_buf), HAL_MAX_DELAY);

	         HAL_Delay(50);
//
//	   /* ======= 3. 在你的控制周期/主循环中修改 ======= */
//
//	      // 更新编码器计数值
//	      Encoder_Update();
//
//	      // 读取当前左右轮速度数据
//	      int16_t speed_L = Encoder_GetLeftSpeed();
//	      int16_t speed_R = Encoder_GetRightSpeed();
//
//	      // 【注释掉原有的打印代码】
//	      // printf("P:%.2f R:%.2f PWM:%d\r\n", current_pitch, current_roll, balance_pwm);
//
//	      // 【替换为以下打印代码：包含左右编码器数值、Pitch角度、Roll角度】
//	      printf("L:%d | R:%d\r\n", speed_L, speed_R);
//
//	      // 保持你原有的控制周期延时（例如 10ms 延时，或者在定时器中断中执行上述逻辑）
//	      HAL_Delay(10);
//     test_count++;
//
//     // 1. 获取最新滤波角度
//     MPU6050_GetAngle(0.05f);
//
//     // 2. 直立环 PD 计算
//     int32_t balance_pwm = Vertical_Loop(MPU_Roll, MPU_Gyro_Y);
//
//     // 3. 驱动电机 (如果控制方向反了，则将参数加负号 "-balance_pwm")
//     Motor_SetSpeed(0, -balance_pwm);
//     Motor_SetSpeed(1, -balance_pwm);
//
//     // 4. 格式化数据打印
//     uint8_t pin_status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
//     char txt_buf[128];
//     int32_t p_show = (int32_t)(MPU_Pitch * 100.0f);
//     int32_t r_show = (int32_t)(MPU_Roll * 100.0f);
//
//     sprintf(txt_buf, "Cnt:%ld | PIN12:%d | P:%ld.%02ld R:%ld.%02ld | PWM:%ld\r\n",
//             test_count,
//             pin_status,
//             p_show / 100, (p_show < 0 ? -p_show : p_show) % 100,
//             r_show / 100, (r_show < 0 ? -r_show : r_show) % 100,
//             balance_pwm);
//
//     HAL_UART_Transmit(&huart1, (uint8_t *)txt_buf, strlen(txt_buf), HAL_MAX_DELAY);
//
//     HAL_Delay(50);

//       test_count++;
//
//       uint8_t pin_status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
//       char txt_buf[128];
//
//       // 调用我们在 mpu6050.c 里已经改好的全软件模拟读取函数！
//       MPU6050_GetAngle(0.01f);
//
//       int32_t p_int = (int32_t)MPU_Pitch;
//       int32_t r_int = (int32_t)MPU_Roll;
//
//       sprintf(txt_buf, "Cnt:%ld | PIN12:%d | P:%ld R:%ld\r\n", test_count, pin_status, p_int, r_int);
//       HAL_UART_Transmit(&huart1, (uint8_t *)txt_buf, strlen(txt_buf), HAL_MAX_DELAY);
//
//       HAL_Delay(50);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
