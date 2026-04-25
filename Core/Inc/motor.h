#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"
#include "tim.h"

#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

void Motor_Init(void);
void Motor_SetSpeed(uint8_t motor,int16_t speed);
void Motor_Stop(uint8_t motor);
void Motor_StopAll(void);

#endif /* INC_MOTOR_H_ */
