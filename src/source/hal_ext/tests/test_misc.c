#include "../../../config/internal.h"
#if SL_USE_HAL

    #include "unity.h"

    #include "../misc.h"

void setUp(void) {
}
void tearDown(void) {
}

void test_sl_hal_periph_classification(void) {
    // APB1
    TEST_ASSERT_TRUE(SL_HAL_IS_APB1_PERIPH(APB1PERIPH_BASE));
    TEST_ASSERT_FALSE(SL_HAL_IS_APB1_PERIPH(APB2PERIPH_BASE));

    // APB2
    TEST_ASSERT_TRUE(SL_HAL_IS_APB2_PERIPH(APB2PERIPH_BASE));

    // AHB1
    TEST_ASSERT_TRUE(SL_HAL_IS_AHB1_PERIPH(AHB1PERIPH_BASE));

    // AHB2
    TEST_ASSERT_TRUE(SL_HAL_IS_AHB2_PERIPH(AHB2PERIPH_BASE));
}

#endif // SL_USE_HAL
