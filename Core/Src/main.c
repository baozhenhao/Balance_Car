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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

volatile uint8_t ctrl_flag = 0;

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
   Motor_Init();
   HAL_TIM_Base_Start_IT(&htim1);


   // =================== 软件 I2C 强行唤醒 MPU6050 开始 ===================
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//
//    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
//
//    uint32_t d;
//    // [Start]
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);   d=30; while(d--);
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); d=30; while(d--);
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=30; while(d--);
//    // [Send 0xD0]
//    uint8_t val1 = 0xD0;
//    for(int i=0; i<8; i++) {
//        if(val1 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//        val1 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//    }
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//    // [Send 0x6B]
//    uint8_t val2 = 0x6B;
//    for(int i=0; i<8; i++) {
//        if(val2 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//        val2 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//    }
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//    // [Send 0x00]
//    uint8_t val3 = 0x00;
//    for(int i=0; i<8; i++) {
//        if(val3 & 0x80) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//        val3 <<= 1; d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//    }
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=15; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); d=15; while(d--);
//    // [Stop]
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); d=30; while(d--); HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); d=30; while(d--);
//    // =================== 软件 I2C 强行唤醒 MPU6050 结束 ===================
//
//    HAL_Delay(100);

    // 3. 开启定时器中断（用来跑闭环控制 Control_Loop，假设用的是定时器 TIM1）
//    HAL_TIM_Base_Start_IT(&htim1);

  /* USER CODE END 2 */


   /* USER CODE BEGIN WHILE */
   uint32_t last_ctrl   = 0;
   uint32_t last_send   = 0;

	    while (1)
   {

	    	if (ctrl_flag)          /* 标志来了才做控制 */
	    	    {
	    	        ctrl_flag = 0;
	    	        MPU6050_GetAngle(0.005f);   /* 读MPU + 滤波 —— 在main上下文，不阻塞系统 */
	    	        /* ===== PID 控制放这里 ===== */
	    	    }
//	    	/* 每 5ms：读MPU + 互补滤波（dt 用真实时间，比写死 0.005f 还准） */
//	    	    if (HAL_GetTick() - last_ctrl >= 5)
//	    	    {
//	    	        float dt = (HAL_GetTick() - last_ctrl) / 1000.0f;
//	    	        last_ctrl = HAL_GetTick();
//	    	        MPU6050_GetAngle(dt);
//	    	    }
	    	    /* 每 50ms：蓝牙发一次 */
	    	    if (HAL_GetTick() - last_send >= 20)
	    	    {
	    	        last_send = HAL_GetTick();
	    	        char buf[80];
	    	        int len = sprintf(buf, "%.2f,%.2f,%.2f,%.2f\r\n",
	    	                          (double)MPU_Pitch, (double)MPU_Roll,
	    	                          (double)Accel_Pitch, (double)Accel_Roll);
	    	        HAL_UART_Transmit(&huart1, (uint8_t*)buf, len, 100);
	    	    }



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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) {


    	ctrl_flag = 1;

    }
}
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
