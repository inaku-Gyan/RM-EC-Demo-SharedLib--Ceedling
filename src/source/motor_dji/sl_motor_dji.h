#pragma once

#include "../../config/internal.h" // IWYU pragma: keep — 提供 SL_USE_FREERTOS

#include <stdint.h>

typedef struct {
    int16_t target_current;
    int16_t actual_current;
    int16_t actual_speed;
    int16_t actual_angle;
} sl_motor_dji_t;

void sl_motor_dji_init(sl_motor_dji_t *motor);
void sl_motor_dji_set_target_current(sl_motor_dji_t *motor, int16_t current);
int16_t sl_motor_dji_get_speed(const sl_motor_dji_t *motor);

#if SL_USE_FREERTOS
/* 实现见 sl_motor_dji_rtos.c。读取 actual_speed 时进入临界区，供多任务调用。 */
int16_t sl_motor_dji_get_speed_threadsafe(sl_motor_dji_t *motor);
#endif
