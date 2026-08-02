#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "main.h"
#include "tim.h"

void Encoder_Init(void);
void Encoder_Update(void);
int16_t Encoder_GetLeftSpeed(void);
int16_t Encoder_GetRightSpeed(void);
extern int16_t enc_left_speed;
extern int16_t enc_right_speed;

#endif /* INC_ENCODER_H_ */
