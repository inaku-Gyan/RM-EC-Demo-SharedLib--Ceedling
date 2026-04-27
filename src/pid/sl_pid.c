#include "sl_pid.h"

/* 对称限幅：limit<=0 时直接放行 */
static float clamp_symmetric(float v, float limit) {
    if (limit <= 0.0f) {
        return v;
    }
    if (v > limit) {
        return limit;
    }
    if (v < -limit) {
        return -limit;
    }
    return v;
}

void sl_pid_init(sl_pid_t *pid, const sl_pid_config_t *cfg) {
    pid->cfg = *cfg;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
}

void sl_pid_reset(sl_pid_t *pid) {
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
}

float sl_pid_update(sl_pid_t *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;

    pid->integral = clamp_symmetric(pid->integral + error * dt, pid->cfg.i_limit);
    float derivative = (dt > 0.0f) ? (error - pid->prev_error) / dt : 0.0f;
    pid->prev_error = error;

    float out = pid->cfg.kp * error
              + pid->cfg.ki * pid->integral
              + pid->cfg.kd * derivative;
    return clamp_symmetric(out, pid->cfg.out_limit);
}
