#pragma once

#include "../../config/internal.h"

#include <stdint.h>

#if SL_USE_HAL && SL_HAL_VERSION_F4

    #include SL_INCLUDE_HAL // IWYU pragma: keep — 提供 TIM_TypeDef / TIM_HandleTypeDef

/**
 * @name 外设总线判断（基于 STM32F4 内存布局，参考 stm32f407xx.h）
 * 取值用 uintptr_t 比较，回避指针 cast-qual。
 * @{
 */
    #define SL_HAL_IS_APB1_PERIPH(periph)                                                          \
        ((uintptr_t)APB1PERIPH_BASE <= (uintptr_t)(periph) &&                                      \
         (uintptr_t)(periph) < (uintptr_t)APB2PERIPH_BASE)
    #define SL_HAL_IS_APB2_PERIPH(periph)                                                          \
        ((uintptr_t)APB2PERIPH_BASE <= (uintptr_t)(periph) &&                                      \
         (uintptr_t)(periph) < (uintptr_t)AHB1PERIPH_BASE)
    #define SL_HAL_IS_AHB1_PERIPH(periph)                                                          \
        ((uintptr_t)AHB1PERIPH_BASE <= (uintptr_t)(periph) &&                                      \
         (uintptr_t)(periph) < (uintptr_t)AHB2PERIPH_BASE)
    #define SL_HAL_IS_AHB2_PERIPH(periph) ((uintptr_t)AHB2PERIPH_BASE <= (uintptr_t)(periph))
/** @} */

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

#endif // SL_USE_HAL && SL_HAL_VERSION_F4
