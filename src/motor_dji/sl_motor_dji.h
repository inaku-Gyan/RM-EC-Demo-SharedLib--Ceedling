#pragma once

#include "sl_config_internal.h" // IWYU pragma: keep — provides SL_USE_FREERTOS used in #if below

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
/* Implemented in sl_motor_dji_rtos.c. Reads protected from concurrent updates
 * via a critical section, intended for multi-task callers. */
int16_t sl_motor_dji_get_speed_threadsafe(sl_motor_dji_t *motor);
#endif
