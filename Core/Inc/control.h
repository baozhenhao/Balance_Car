// control.h
#ifndef __CONTROL_H
#define __CONTROL_H

#include "main.h"

// 暴露 PID 参数，这样你在 main.c 里可以通过代码直接修改它们进行调参
extern float Kp_angle, Kd_angle;
extern float Kp_speed, Ki_speed;

// 控制函数原型
void Control_Loop(float Actual_Angle, float Gyro_Y, int16_t enc_left, int16_t enc_right);

#endif
