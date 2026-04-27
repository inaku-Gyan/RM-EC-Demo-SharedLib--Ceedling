#include "sl_config_internal.h"

/* Case-1 file: requires FreeRTOS. Tests must conditionally pull this TU into
 * the build (via `#if SL_USE_FREERTOS` around TEST_SOURCE_FILE), otherwise the
 * #error fires. */
#if !SL_USE_FREERTOS
#error "sl_motor_dji_rtos.c requires SL_USE_FREERTOS=1"
#endif

#include "sl_motor_dji.h"

int16_t sl_motor_dji_get_speed_threadsafe(sl_motor_dji_t *motor) {
    int16_t s;
    taskENTER_CRITICAL();
    s = motor->actual_speed;
    taskEXIT_CRITICAL();
    return s;
}
