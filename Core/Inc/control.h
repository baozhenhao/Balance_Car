#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include "pid.h"

/* 供蓝牙调参直接访问（定义在 control.c） */
extern PID_t balance_pid;   /* 直立环 */
extern PID_t speed_pid;     /* 速度环 */
extern float angle_target;  /* 机械中值(tr) */

void Control_Init(void);              /* 初始化两个环 */
void Control_BalanceLoop(float dt);   /* 直立环：5ms 调一次 */
void Control_SpeedLoop(float dt);     /* 速度环：50ms 调一次 */

/* 供 main 打印调试 */
float Control_GetBalanceOut(void);
float Control_GetAngleCmd(void);

#endif /* INC_CONTROL_H_ */
