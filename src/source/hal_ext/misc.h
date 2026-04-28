#pragma once

#include "../../config/internal.h"
#if SL_USE_HAL // 仅当启用 HAL 时才编译本文件

    #include <stdint.h> // 提供 uintptr_t

    #include SL_INCLUDE_HAL

    /**
 * @name 外设总线判断（基于 STM32F4 内存布局，参考 stm32f407xx.h）
 * 取值用 uintptr_t 比较，回避指针 cast-qual。
 * @{
 */

    /// 判断外设所属总线是否为 APB1
    #define SL_HAL_IS_APB1_PERIPH(periph)                                                          \
        ((uintptr_t)APB1PERIPH_BASE <= (uintptr_t)(periph) &&                                      \
         (uintptr_t)(periph) < (uintptr_t)APB2PERIPH_BASE)

    /// 判断外设所属总线是否为 APB2
    #define SL_HAL_IS_APB2_PERIPH(periph)                                                          \
        ((uintptr_t)APB2PERIPH_BASE <= (uintptr_t)(periph) &&                                      \
         (uintptr_t)(periph) < (uintptr_t)AHB1PERIPH_BASE)

    /// 判断外设所属总线是否为 AHB1
    #define SL_HAL_IS_AHB1_PERIPH(periph)                                                          \
        ((uintptr_t)AHB1PERIPH_BASE <= (uintptr_t)(periph) &&                                      \
         (uintptr_t)(periph) < (uintptr_t)AHB2PERIPH_BASE)

    /// 判断外设所属总线是否为 AHB2
    #define SL_HAL_IS_AHB2_PERIPH(periph) ((uintptr_t)AHB2PERIPH_BASE <= (uintptr_t)(periph))

/** @} */

#endif // SL_USE_HAL