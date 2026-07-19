// control.c
#include "control.h"
// 声明 MPU6050 相关的全局变量（根据你 main.c 里的定义）
#include "control.h"
#include "encoder.h"   // 1. 包含编码器头文件，解决 Encoder_Update 警告
#include "mpu6050.h"   // 2. 包含陀螺仪头文件，解决 MPU6050_GetAngle 警告

// 3. 声明你实际的电机控制外部函数 (根据你 main.c 里的 Motor_SetSpeed 声明)[span_4](start_span)[span_4](end_span)
extern void Motor_SetSpeed(uint8_t motor, int16_t speed);
// 参数初始化
float Kp_angle = 60.0f;
float Kd_angle = 0.23f;
float Kp_speed = -0.15f;
float Ki_speed = -0.0012f;

float speed_integral = 0;
float target_speed = 0;

void Control_Loop(float Actual_Angle, float Gyro_Y, int16_t enc_left, int16_t enc_right) {
    static uint8_t speed_cnt = 0;
    static float angle_offset = 0;

    // 1. 速度环 (50ms)
    if(++speed_cnt >= 5) {
        speed_cnt = 0;
        float current_speed = (enc_left + enc_right) / 2.0f;
        float speed_error = target_speed - current_speed;
        speed_integral += speed_error;
        if(speed_integral > 3000) speed_integral = 3000;
        if(speed_integral < -3000) speed_integral = -3000;
        angle_offset = Kp_speed * speed_error + Ki_speed * speed_integral;
    }

    // 2. 直立环 + 速度环合并输出
    float final_target_angle = 2.4f + angle_offset;
    float PWM_Out = -(Kp_angle * (Actual_Angle - final_target_angle) + Kd_angle * Gyro_Y);

    // 3. 输出限幅 (假设你的ARR是7200)
    if(PWM_Out > 7000) PWM_Out = 7000;
    if(PWM_Out < -7000) PWM_Out = -7000;

    // ✅ 使用你原本就有的左右电机驱动函数[span_6](start_span)[span_6](end_span)
    // 注意：如果小车方向反了，可以在 PWM_Out 前面加负号，比如 -PWM_Out[span_7](start_span)[span_7](end_span)
    Motor_SetSpeed(0, (int16_t)PWM_Out); // 驱动左电机
    Motor_SetSpeed(1, (int16_t)PWM_Out); // 驱动右电机
}
// 声明 MPU6050 相关的全局变量（根据你 main.c 里的定义）
extern float MPU_Roll;
extern float MPU_Gyro_Y;
extern int16_t enc_left_speed;
extern int16_t enc_right_speed;

// 手动实现定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 检查触发中断的是不是你的控制定时器（假设是 TIM1）
    if (htim->Instance == TIM1)
    {
        // 1. 获取最新滤波角度（10ms控制周期，传入 0.01f）
        MPU6050_GetAngle(0.01f);

        // 2. 更新编码器计数值
        Encoder_Update();

        // 3. 执行直立+速度串级控制（使用你之前调好的 MPU_Roll）
        Control_Loop(MPU_Roll, MPU_Gyro_Y, enc_left_speed, enc_right_speed);
    }
}
