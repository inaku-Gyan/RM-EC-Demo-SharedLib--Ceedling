#include "../../config/internal.h" // IWYU pragma: keep — 提供 SL_USE_HAL / SL_HAL_VERSION_F4
#include "unity.h"
#include "sl_hal_utils.h"

#if SL_USE_HAL && SL_HAL_VERSION_F4
TEST_SOURCE_FILE("sl_hal_utils_f4.c")
#endif

void setUp(void) {
}
void tearDown(void) {
}

#if SL_USE_HAL && SL_HAL_VERSION_F4

    #include SL_INCLUDE_HAL // IWYU pragma: keep — 提供 TIM/RCC 桩定义

void test_sl_hal_tim_set_duty_cycle_clamps_high(void) {
    TIM_TypeDef tim_inst = {.ARR = 1000};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_1, 1.5f);
    TEST_ASSERT_EQUAL_UINT32(1000u, tim_inst.CCR1);
}

void test_sl_hal_tim_set_duty_cycle_clamps_low(void) {
    TIM_TypeDef tim_inst = {.ARR = 1000};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_2, -0.5f);
    TEST_ASSERT_EQUAL_UINT32(0u, tim_inst.CCR2);
}

void test_sl_hal_tim_set_duty_cycle_half(void) {
    TIM_TypeDef tim_inst = {.ARR = 1000};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_3, 0.5f);
    TEST_ASSERT_EQUAL_UINT32(500u, tim_inst.CCR3);
}

// APB1 分频 == 1：TIM 时钟 = PCLK1 = 42 MHz（桩默认）
void test_sl_hal_tim_get_clock_freq_apb1_div1(void) {
    // 在 APB1 区间内（[0x40000000, 0x40010000)）构造一个伪实例地址
    TIM_TypeDef *fake_apb1 = (TIM_TypeDef *)APB1PERIPH_BASE;
    TEST_ASSERT_EQUAL_UINT32(42000000u, sl_hal_tim_get_clock_freq_hz(fake_apb1));
}

// APB2 分频 == 1：TIM 时钟 = PCLK2 = 84 MHz（桩默认）
void test_sl_hal_tim_get_clock_freq_apb2_div1(void) {
    TIM_TypeDef *fake_apb2 = (TIM_TypeDef *)APB2PERIPH_BASE;
    TEST_ASSERT_EQUAL_UINT32(84000000u, sl_hal_tim_get_clock_freq_hz(fake_apb2));
}

void test_sl_hal_is_apb1_periph_classification(void) {
    TEST_ASSERT_TRUE(SL_HAL_IS_APB1_PERIPH((TIM_TypeDef *)APB1PERIPH_BASE));
    TEST_ASSERT_FALSE(SL_HAL_IS_APB1_PERIPH((TIM_TypeDef *)APB2PERIPH_BASE));
    TEST_ASSERT_TRUE(SL_HAL_IS_APB2_PERIPH((TIM_TypeDef *)APB2PERIPH_BASE));
    TEST_ASSERT_TRUE(SL_HAL_IS_AHB1_PERIPH((TIM_TypeDef *)AHB1PERIPH_BASE));
    TEST_ASSERT_TRUE(SL_HAL_IS_AHB2_PERIPH((TIM_TypeDef *)AHB2PERIPH_BASE));
}

// 栈上 tim_inst 地址远超 APB1 区间，总线判断走 APB2 分支：桩 PCLK2 = 84MHz、
// APB2 分频 == 1 → TIM 时钟 = 84MHz；PSC = 0 → freq = 84MHz。
// pulse = 100us × 84MHz / 1e6 = 8400
void test_sl_hal_tim_set_pulse_width_us_basic(void) {
    TIM_TypeDef tim_inst = {.PSC = 0, .ARR = 10000};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};

    sl_hal_tim_set_pulse_width_us(&htim, TIM_CHANNEL_1, 100.0f);
    TEST_ASSERT_EQUAL_UINT32(8400u, tim_inst.CCR1);
}

void test_sl_hal_tim_set_pulse_width_us_clamps_to_arr(void) {
    TIM_TypeDef tim_inst = {.PSC = 0, .ARR = 1000};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};

    sl_hal_tim_set_pulse_width_us(&htim, TIM_CHANNEL_2, 1.0e6f);
    TEST_ASSERT_EQUAL_UINT32(1000u, tim_inst.CCR2);
}

#endif // SL_USE_HAL && SL_HAL_VERSION_F4
