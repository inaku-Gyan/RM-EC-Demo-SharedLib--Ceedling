#include "unity.h"
#include "sl_motor_dji.h"

void setUp(void) {}
void tearDown(void) {}

void test_sl_motor_dji_init_zeroes_struct(void) {
    sl_motor_dji_t m = {.target_current = 5, .actual_speed = 7};
    sl_motor_dji_init(&m);
    TEST_ASSERT_EQUAL_INT16(0, m.target_current);
    TEST_ASSERT_EQUAL_INT16(0, m.actual_speed);
}

void test_sl_motor_dji_set_target_current(void) {
    sl_motor_dji_t m;
    sl_motor_dji_init(&m);
    sl_motor_dji_set_target_current(&m, 1234);
    TEST_ASSERT_EQUAL_INT16(1234, m.target_current);
}

void test_sl_motor_dji_get_speed(void) {
    sl_motor_dji_t m;
    sl_motor_dji_init(&m);
    m.actual_speed = -42;
    TEST_ASSERT_EQUAL_INT16(-42, sl_motor_dji_get_speed(&m));
}
