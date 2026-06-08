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
    // 1. 直接读取这段时间内的脉冲变化量（强制转成 int16_t 处理正反转的负数溢出）
    int16_t cur_left = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    int16_t cur_right = (int16_t)__HAL_TIM_GET_COUNTER(&htim4);

    // 2. 赋值给全局速度变量（注意：左右轮安装方向通常相反，如果发现车往前推有个轮子速度是负的，直接在这里加个负号 -cur_right 就行）
    enc_left_speed = cur_left;
    enc_right_speed = cur_right; // 如果接线反了，改成 -cur_right

    // 3. 读完立刻把硬件计数器清零，为下一次 5ms 的统计做准备！
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
}

int16_t Encoder_GetLeftSpeed(void)
{
	return enc_left_speed;
}

int16_t Encoder_GetRightSpeed(void)
{
	return enc_right_speed;
}























