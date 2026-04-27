#include "sl_motor_dji.h"

#include <string.h>

void sl_motor_dji_init(sl_motor_dji_t *motor) {
    memset(motor, 0, sizeof(*motor));
}

void sl_motor_dji_set_target_current(sl_motor_dji_t *motor, int16_t current) {
    motor->target_current = current;
}

int16_t sl_motor_dji_get_speed(const sl_motor_dji_t *motor) {
    return motor->actual_speed;
}
