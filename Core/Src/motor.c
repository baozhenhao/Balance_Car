#include "motor.h"

void Motor_Init(void)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,0);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,0);
}

void Motor_SetSpeed(uint8_t motor,int16_t speed)
{
	if(speed>999) speed=999;
	if(speed<-999) speed=-999;

	if(motor == MOTOR_LEFT){
			if(speed>=0){
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,speed);
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);
			}else {
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,0);
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,-speed);
			}
	}else if(motor == MOTOR_RIGHT){
		if(speed>=0){
						__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,speed);
						__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,0);
					}else {
						__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,0);
						__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,-speed);
					}
	}

}

void Motor_Stop(uint8_t motor)
{
	Motor_SetSpeed(motor,0);
}


void Motor_StopAll(void)
{
	Motor_Stop(MOTOR_LEFT);
	Motor_Stop(MOTOR_RIGHT);
}





