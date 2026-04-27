#include "encoder.h"

static int16_t enc_left_speed=0;
static int16_t enc_right_speed=0;

void Encoder_Init(void)
{
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
}

void Encoder_Update(void)
{
	static int16_t last_left_cnt=0;
	static int16_t last_right_cnt=0;

	int16_t cur_left = __HAL_TIM_GET_COUNTER(&htim3);
	int16_t cur_right = __HAL_TIM_GET_COUNTER(&htim4);

	enc_left_speed = cur_left - last_left_cnt;
	enc_right_speed = cur_right - last_right_cnt;

	last_left_cnt=cur_left;
	last_right_cnt=cur_right;

	__HAL_TIM_SET_COUNTER(&htim3,0);
	__HAL_TIM_SET_COUNTER(&htim4,0);
}

int16_t Encoder_GetLeftSpeed(void)
{
	return enc_left_speed;
}

int16_t Encoder_GetRightSpeed(void)
{
	return enc_right_speed;
}























