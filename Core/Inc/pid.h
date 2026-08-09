
#ifndef INC_PID_H_
#define INC_PID_H_
#include <stdint.h>
typedef struct {
    float kp, ki, kd;
    float out_min, out_max;
    float integral;
    float last_err;
    uint8_t use_integral;
} PID_t;
void PID_Init(PID_t *pid, float kp, float ki, float kd,
              float out_min, float out_max);
void PID_Reset(PID_t *pid);
float PID_Update(PID_t *pid, float target, float measure, float dt);
void PID_SetGains(PID_t *pid, float kp, float ki, float kd);

#endif /* INC_PID_H_ */
