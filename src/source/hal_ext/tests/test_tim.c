#include "../../../config/internal.h"
#if SL_USE_HAL

    #include "unity.h"

    #include "../tim.h"
TEST_SOURCE_FILE("../tim.c")

    // CMock 解析 stm32f4xx_hal_rcc.h 生成 mock_stm32f4xx_hal_rcc.{h,c}，提供
    // HAL_RCC_* 函数的 Expect/Return API；mock .c 在链接期为 tim.c 提供 HAL_RCC_*
    // 符号定义，省去 host 端再写桩。
    #include "mock_stm32f4xx_hal_rcc.h"

void setUp(void) {
}

void tearDown(void) {
}

// ---- sl_hal_tim_get_clock_freq_hz ---------------------------------------
// TIM2 在 APB1 总线，TIM1 在 APB2；APB 分频 == 1 时 TIM 时钟 = PCLK，
// 否则 = 2 × PCLK（STM32 RCC 规则）。

void test_get_clock_freq_apb1_div1_returns_pclk1(void) {
    RCC_ClkInitTypeDef cfg = {.APB1CLKDivider = RCC_HCLK_DIV1};
    HAL_RCC_GetClockConfig_Expect(NULL, NULL);
    HAL_RCC_GetClockConfig_IgnoreArg_RCC_ClkInitStruct();
    HAL_RCC_GetClockConfig_IgnoreArg_pFLatency();
    HAL_RCC_GetClockConfig_ReturnThruPtr_RCC_ClkInitStruct(&cfg);
    HAL_RCC_GetPCLK1Freq_ExpectAndReturn(84000000U);

    TEST_ASSERT_EQUAL_UINT32(84000000U, sl_hal_tim_get_clock_freq_hz(TIM2));
}

void test_get_clock_freq_apb1_div2_returns_double_pclk1(void) {
    RCC_ClkInitTypeDef cfg = {.APB1CLKDivider = RCC_HCLK_DIV2};
    HAL_RCC_GetClockConfig_Expect(NULL, NULL);
    HAL_RCC_GetClockConfig_IgnoreArg_RCC_ClkInitStruct();
    HAL_RCC_GetClockConfig_IgnoreArg_pFLatency();
    HAL_RCC_GetClockConfig_ReturnThruPtr_RCC_ClkInitStruct(&cfg);
    HAL_RCC_GetPCLK1Freq_ExpectAndReturn(42000000U);

    TEST_ASSERT_EQUAL_UINT32(84000000U, sl_hal_tim_get_clock_freq_hz(TIM2));
}

void test_get_clock_freq_apb2_div1_returns_pclk2(void) {
    RCC_ClkInitTypeDef cfg = {.APB2CLKDivider = RCC_HCLK_DIV1};
    HAL_RCC_GetClockConfig_Expect(NULL, NULL);
    HAL_RCC_GetClockConfig_IgnoreArg_RCC_ClkInitStruct();
    HAL_RCC_GetClockConfig_IgnoreArg_pFLatency();
    HAL_RCC_GetClockConfig_ReturnThruPtr_RCC_ClkInitStruct(&cfg);
    HAL_RCC_GetPCLK2Freq_ExpectAndReturn(168000000U);

    TEST_ASSERT_EQUAL_UINT32(168000000U, sl_hal_tim_get_clock_freq_hz(TIM1));
}

void test_get_clock_freq_apb2_div4_returns_double_pclk2(void) {
    RCC_ClkInitTypeDef cfg = {.APB2CLKDivider = RCC_HCLK_DIV4};
    HAL_RCC_GetClockConfig_Expect(NULL, NULL);
    HAL_RCC_GetClockConfig_IgnoreArg_RCC_ClkInitStruct();
    HAL_RCC_GetClockConfig_IgnoreArg_pFLatency();
    HAL_RCC_GetClockConfig_ReturnThruPtr_RCC_ClkInitStruct(&cfg);
    HAL_RCC_GetPCLK2Freq_ExpectAndReturn(84000000U);

    TEST_ASSERT_EQUAL_UINT32(168000000U, sl_hal_tim_get_clock_freq_hz(TIM1));
}

// ---- sl_hal_tim_set_duty_cycle ------------------------------------------
// TIM_HandleTypeDef.Instance 在 host 上指向本地内存，__HAL_TIM_GET_AUTORELOAD
// / __HAL_TIM_SET_COMPARE 直接读写该结构的 ARR / CCRn 字段——不需要 mock。

void test_set_duty_cycle_normal_writes_proportional_compare(void) {
    TIM_TypeDef tim_inst = {0};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};
    tim_inst.ARR = 1000;

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_1, 0.5f);

    TEST_ASSERT_EQUAL_UINT32(500U, tim_inst.CCR1);
}

void test_set_duty_cycle_zero_writes_zero(void) {
    TIM_TypeDef tim_inst = {0};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};
    tim_inst.ARR = 1000;
    tim_inst.CCR1 = 0xDEADBEEFU; // 起始脏值

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_1, 0.0f);

    TEST_ASSERT_EQUAL_UINT32(0U, tim_inst.CCR1);
}

void test_set_duty_cycle_one_writes_full_period(void) {
    TIM_TypeDef tim_inst = {0};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};
    tim_inst.ARR = 1000;

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_1, 1.0f);

    TEST_ASSERT_EQUAL_UINT32(1000U, tim_inst.CCR1);
}

void test_set_duty_cycle_clamps_negative_to_zero(void) {
    TIM_TypeDef tim_inst = {0};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};
    tim_inst.ARR = 1000;

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_1, -0.5f);

    TEST_ASSERT_EQUAL_UINT32(0U, tim_inst.CCR1);
}

void test_set_duty_cycle_clamps_above_one_to_period(void) {
    TIM_TypeDef tim_inst = {0};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};
    tim_inst.ARR = 1000;

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_1, 1.5f);

    TEST_ASSERT_EQUAL_UINT32(1000U, tim_inst.CCR1);
}

void test_set_duty_cycle_routes_channel_to_correct_ccr(void) {
    TIM_TypeDef tim_inst = {0};
    TIM_HandleTypeDef htim = {.Instance = &tim_inst};
    tim_inst.ARR = 1000;

    sl_hal_tim_set_duty_cycle(&htim, TIM_CHANNEL_3, 0.25f);

    TEST_ASSERT_EQUAL_UINT32(0U, tim_inst.CCR1);
    TEST_ASSERT_EQUAL_UINT32(0U, tim_inst.CCR2);
    TEST_ASSERT_EQUAL_UINT32(250U, tim_inst.CCR3);
    TEST_ASSERT_EQUAL_UINT32(0U, tim_inst.CCR4);
}

#endif // SL_USE_HAL
