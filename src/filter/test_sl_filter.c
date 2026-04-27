#include "unity.h"
#include "sl_filter.h"

void setUp(void) {}
void tearDown(void) {}

void test_sl_filter_dot_basic(void) {
    float a[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float b[4] = {2.0f, 0.0f, 1.0f, -1.0f};
    /* 1*2 + 2*0 + 3*1 + 4*(-1) = 1 */
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 1.0f, sl_filter_dot(a, b, 4));
}

void test_sl_filter_dot_empty(void) {
    float a[1] = {7.0f};
    float b[1] = {9.0f};
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 0.0f, sl_filter_dot(a, b, 0));
}
