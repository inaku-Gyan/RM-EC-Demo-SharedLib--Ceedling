#pragma once

#include "../../config/internal.h"
#if SL_USE_HAL // 仅当启用 HAL 时才编译本文件

    #include <stdint.h>

    #include SL_INCLUDE_HAL

/**
 * @brief  获取定时器输入时钟频率（Hz）
 * @param  tim 定时器实例（如 htimX.Instance）
 * @return TIM 时钟频率，APB 分频 == 1 时为 PCLK，否则为 2 × PCLK
 */
uint32_t sl_hal_tim_get_clock_freq_hz(const TIM_TypeDef *tim);

/**
 * @brief 设置 TIM 通道占空比
 * @param htim    定时器句柄（如 &htim8）
 * @param channel 输出通道（如 TIM_CHANNEL_3）
 * @param duty    占空比；超出 [0.0f, 1.0f] 自动夹紧
 */
void sl_hal_tim_set_duty_cycle(TIM_HandleTypeDef *htim, uint32_t channel, float duty);

/**
 * @brief 设置 TIM 通道脉冲宽度（微秒）
 * @param htim     定时器句柄（如 &htim8）
 * @param channel  输出通道（如 TIM_CHANNEL_3）
 * @param width_us 脉宽，单位 us；超过 ARR 周期自动夹紧到周期
 */
void sl_hal_tim_set_pulse_width_us(TIM_HandleTypeDef *htim, uint32_t channel, float width_us);

#endif // SL_USE_HAL