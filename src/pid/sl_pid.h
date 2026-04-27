#pragma once

typedef struct {
    float kp;
    float ki;
    float kd;
    float i_limit;   /* 积分限幅；0 表示不限 */
    float out_limit; /* 输出限幅；0 表示不限 */
} sl_pid_config_t;

typedef struct {
    sl_pid_config_t cfg;
    float integral;
    float prev_error;
} sl_pid_t;

void sl_pid_init(sl_pid_t *pid, const sl_pid_config_t *cfg);
void sl_pid_reset(sl_pid_t *pid);
float sl_pid_update(sl_pid_t *pid, float setpoint, float measurement, float dt);
