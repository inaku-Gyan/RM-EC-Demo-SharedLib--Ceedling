#pragma once

typedef struct {
    float kp;
    float ki;
    float kd;
    float i_limit;   /* 0 disables integral clamp */
    float out_limit; /* 0 disables output clamp */
} sl_pid_config_t;

typedef struct {
    sl_pid_config_t cfg;
    float integral;
    float prev_error;
} sl_pid_t;

void sl_pid_init(sl_pid_t *pid, const sl_pid_config_t *cfg);
void sl_pid_reset(sl_pid_t *pid);
float sl_pid_update(sl_pid_t *pid, float setpoint, float measurement, float dt);
