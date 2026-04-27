#include "../../config/internal.h" // IWYU pragma: keep — 提供 SL_USE_FREERTOS
#include "unity.h"
#include "sl_motor_dji.h"

/* sl_motor_dji_rtos.c 是 case-1 文件——只在 SL_USE_FREERTOS=1 时挂入构建。
 * 把 TEST_SOURCE_FILE 关进 #if 内，否则非 RTOS mixin 也会编译那个 .c 而触发 #error。 */
#if SL_USE_FREERTOS
TEST_SOURCE_FILE("sl_motor_dji_rtos.c")
#endif

/* setUp / tearDown 放在文件级 #if 之外，否则 mixin 把所有用例都门卫掉时
 * runner 仍引用这两个符号，链接失败。 */
void setUp(void) {
}
void tearDown(void) {
}

#if SL_USE_FREERTOS

void test_sl_motor_dji_threadsafe_speed_returns_actual(void) {
    sl_motor_dji_t m;
    sl_motor_dji_init(&m);
    m.actual_speed = 123;
    TEST_ASSERT_EQUAL_INT16(123, sl_motor_dji_get_speed_threadsafe(&m));
}

#endif /* SL_USE_FREERTOS */
