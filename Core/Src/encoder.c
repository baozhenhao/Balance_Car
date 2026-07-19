#include "encoder.h"
#include "main.h"
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

int16_t enc_left_speed = 0;
int16_t enc_right_speed = 0;

void Encoder_Init(void) {
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
}
void Encoder_Update(void) {
    enc_left_speed = -(int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    enc_right_speed = (int16_t)__HAL_TIM_GET_COUNTER(&htim4); // 如果方向反了，这里加个负号
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
}
int16_t Encoder_GetLeftSpeed(void) { return enc_left_speed; }
int16_t Encoder_GetRightSpeed(void) { return enc_right_speed; }
