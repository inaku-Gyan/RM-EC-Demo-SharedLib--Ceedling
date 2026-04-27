#include "sl_config_internal.h" // IWYU pragma: keep — provides SL_USE_FREERTOS used in #if below
#include "unity.h"
#include "sl_motor_dji.h"       // hoisted out of #if so Ceedling's source-file
                                // discovery sees it and links sl_motor_dji.c

/* Case-1 source: only attach sl_motor_dji_rtos.c to the build when its
 * dependency is actually enabled. Outside #if SL_USE_FREERTOS, the rtos TU
 * would #error during compile. */
#if SL_USE_FREERTOS
TEST_SOURCE_FILE("sl_motor_dji_rtos.c")
#endif

/* setUp/tearDown live outside the guard so the test runner always links,
 * even under mixins that strip every test case in this file. */
void setUp(void) {}
void tearDown(void) {}

#if SL_USE_FREERTOS

void test_sl_motor_dji_threadsafe_speed_returns_actual(void) {
    sl_motor_dji_t m;
    sl_motor_dji_init(&m);
    m.actual_speed = 123;
    TEST_ASSERT_EQUAL_INT16(123, sl_motor_dji_get_speed_threadsafe(&m));
}

#endif /* SL_USE_FREERTOS */
