#include "unity.h"
#include "sl_pid.h"

void setUp(void) {
}
void tearDown(void) {
}

void test_sl_pid_proportional_only(void) {
    sl_pid_config_t cfg = {.kp = 2.0f};
    sl_pid_t pid;
    sl_pid_init(&pid, &cfg);

    float out = sl_pid_update(&pid, 10.0f, 4.0f, 0.01f);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 12.0f, out);
}

void test_sl_pid_output_clamp(void) {
    sl_pid_config_t cfg = {.kp = 100.0f, .out_limit = 50.0f};
    sl_pid_t pid;
    sl_pid_init(&pid, &cfg);

    float out = sl_pid_update(&pid, 10.0f, 0.0f, 0.01f);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 50.0f, out);
}

void test_sl_pid_integral_clamp_symmetric(void) {
    sl_pid_config_t cfg = {.ki = 1.0f, .i_limit = 5.0f};
    sl_pid_t pid;
    sl_pid_init(&pid, &cfg);

    for (int i = 0; i < 100; ++i) {
        sl_pid_update(&pid, 10.0f, 0.0f, 1.0f);
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 5.0f, pid.integral);

    for (int i = 0; i < 100; ++i) {
        sl_pid_update(&pid, -10.0f, 0.0f, 1.0f);
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, -5.0f, pid.integral);
}

void test_sl_pid_reset_zeroes_state(void) {
    sl_pid_config_t cfg = {.kp = 1.0f, .ki = 1.0f, .i_limit = 100.0f};
    sl_pid_t pid;
    sl_pid_init(&pid, &cfg);
    sl_pid_update(&pid, 10.0f, 0.0f, 1.0f);

    sl_pid_reset(&pid);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.0f, pid.integral);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.0f, pid.prev_error);
}
