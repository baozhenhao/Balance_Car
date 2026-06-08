#include "motor.h"
#include "main.h"   // 必须包含 main.h，才能识别 GPIOA 和 GPIO_PIN_4 这些名字！

// 外部引用定时器2 (如果你在 .h 里写了这里就不用管)
extern TIM_HandleTypeDef htim2;

void Motor_Init(void)
{
    // 我们只需要 2 个 PWM 通道！(通道1控左轮，通道2控右轮)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    // 初始速度设为 0
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
}

void Motor_SetSpeed(uint8_t motor, int16_t speed)
{
    // 1. 速度限幅 (防止越界，假设你的定时器最大重装载值是 999)
    if(speed > 999) speed = 999;
    if(speed < -999) speed = -999;

    // 2. 左轮控制逻辑
    if(motor == MOTOR_LEFT) {
        if(speed >= 0) {
            // 前进：IA1(PA4)=1, IA2(PA5)=0
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
            // 踩油门
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
        } else {
            // 后退：IA1(PA4)=0, IA2(PA5)=1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            // 踩油门 (注意负数要加负号变正数传给PWM)
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, -speed);
        }
    }
    // 3. 右轮控制逻辑
    else if(motor == MOTOR_RIGHT) {
        if(speed >= 0) {
            // 前进：IB1(PB0)=1, IB2(PB1)=0
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
            // 踩油门
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
        } else {
            // 后退：IB1(PB0)=0, IB2(PB1)=1
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
            // 踩油门 (注意负数变正数)
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, -speed);
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





