#include "tim.h"
#if SL_USE_HAL // 仅当启用 HAL 时才编译本文件

    #include "misc.h"

uint32_t sl_hal_tim_get_clock_freq_hz(const TIM_TypeDef *tim) {
    SL_ASSERT(IS_TIM_INSTANCE(tim));

    RCC_ClkInitTypeDef clk_cfg = {0};
    uint32_t latency = 0; // HAL_RCC_GetClockConfig 必填出参，本函数不读
    HAL_RCC_GetClockConfig(&clk_cfg, &latency);

    uint32_t pclk;
    uint32_t apb_div;
    if (SL_HAL_IS_APB1_PERIPH(tim)) {
        // APB1 总线（TIM2/3/4/5/6/7/12/13/14）
        pclk = HAL_RCC_GetPCLK1Freq();
        apb_div = clk_cfg.APB1CLKDivider;
    } else {
        // APB2 总线（TIM1/8/9/10/11）
        pclk = HAL_RCC_GetPCLK2Freq();
        apb_div = clk_cfg.APB2CLKDivider;
    }

    // STM32 RCC：APB 分频 == 1 时 TIM 时钟 = PCLK，否则 = 2 × PCLK
    if (apb_div == RCC_HCLK_DIV1) {
        return pclk;
    }
    return pclk * 2u;
}

void sl_hal_tim_set_duty_cycle(TIM_HandleTypeDef *htim, uint32_t channel, float duty) {
    SL_ASSERT(htim != NULL);

    if (duty < 0.0f) {
        duty = 0.0f;
    } else if (duty > 1.0f) {
        duty = 1.0f;
    }

    const uint32_t period = __HAL_TIM_GET_AUTORELOAD(htim);
    const uint32_t pulse = (uint32_t)((float)period * duty);

    __HAL_TIM_SET_COMPARE(htim, channel, pulse);
}

void sl_hal_tim_set_pulse_width_us(TIM_HandleTypeDef *htim, uint32_t channel, float width_us) {
    SL_ASSERT(htim != NULL);
    SL_ASSERT(width_us >= 0.0f);

    const uint32_t tim_clock = sl_hal_tim_get_clock_freq_hz(htim->Instance);
    const uint32_t prescaler = htim->Instance->PSC;
    const float freq = (float)tim_clock / (float)(prescaler + 1u);

    // count = width_us × freq / 1_000_000
    uint32_t pulse = (uint32_t)(width_us * freq / 1000000.0f);

    const uint32_t period = __HAL_TIM_GET_AUTORELOAD(htim);
    if (pulse > period) {
        pulse = period;
    }

    __HAL_TIM_SET_COMPARE(htim, channel, pulse);
}

#endif // SL_USE_HAL