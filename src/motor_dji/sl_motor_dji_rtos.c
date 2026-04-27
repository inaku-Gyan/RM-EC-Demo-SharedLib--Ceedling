/* Case 1：本文件强制要求 SL_USE_FREERTOS=1。配套的测试文件必须用
 * #if SL_USE_FREERTOS 包裹 TEST_SOURCE_FILE，避免无依赖时被强行编译并触发 #error。 */
#include "../../config/internal.h"

#if SL_USE_FREERTOS

    #include SL_INCLUDE_FREERTOS // IWYU pragma: keep — FreeRTOS 要求本头先于其他 FreeRTOS 头出现
    #include SL_INCLUDE_FREERTOS_TASK
    #include "sl_motor_dji.h"

    #include <stdint.h>

int16_t sl_motor_dji_get_speed_threadsafe(sl_motor_dji_t *motor) {
    int16_t s;
    taskENTER_CRITICAL();
    s = motor->actual_speed;
    taskEXIT_CRITICAL();
    return s;
}

#else
    #error "sl_motor_dji_rtos.c 需要 SL_USE_FREERTOS=1"
#endif
