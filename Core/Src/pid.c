#include "pid.h"

void PID_Init(PID_t *pid, float kp, float ki, float kd,
              float out_min, float out_max)
{
    pid->kp = kp; pid->ki = ki; pid->kd = kd;
    pid->out_min = out_min; pid->out_max = out_max;
    pid->use_integral = 1;
    PID_Reset(pid);
}

void PID_Reset(PID_t *pid)
{
    pid->integral = 0.0f; pid->last_err = 0.0f;
}

float PID_Update(PID_t *pid, float target, float measure, float dt)
{
    float err = target - measure;
    float P = pid->kp * err;

    float I = 0.0f;
    if (pid->use_integral && pid->ki != 0.0f) {
        pid->integral += err * dt;
        float i_lim = pid->out_max / pid->ki;
        if (pid->integral >  i_lim) pid->integral =  i_lim;
        if (pid->integral < -i_lim) pid->integral = -i_lim;
        I = pid->ki * pid->integral;
    }

    float D = 0.0f;
    if (dt > 0.0f) {
        D = pid->kd * (err - pid->last_err) / dt;
        pid->last_err = err;
    }

    float out = P + I + D;
    if (out > pid->out_max) out = pid->out_max;
    if (out < pid->out_min) out = pid->out_min;
    return out;
}
void PID_SetGains(PID_t *pid, float kp, float ki, float kd)
{
    pid->kp = kp; pid->ki = ki; pid->kd = kd;
    PID_Reset(pid);   /* 换参后清零积分/历史，防止跳变 */
}
