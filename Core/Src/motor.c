#include "motor.h"
#include "main.h"
extern TIM_HandleTypeDef htim2;

void Motor_Init(void) {
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

void Motor_SetSpeed(uint8_t motor, int16_t speed) {
    if(speed > 999) speed = 999;
    if(speed < -999) speed = -999;

    if(motor == 0) { // LEFT
        if(speed >= 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
        } else {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, -speed);
        }
    } else { // RIGHT
        if(speed >= 0) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
        } else {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, -speed);
        }
    }
}
void Motor_StopAll(void) {
    Motor_SetSpeed(0, 0); Motor_SetSpeed(1, 0);
}
