#pragma once

/* 测试期 STM32F4 HAL 桩头。提供 sl_hal_utils 编译/链接所需的最小定义。
 * 真实集成时业务方把 SL_INCLUDE_HAL 指向 STM32F4xx_HAL_Driver 的真实头文件。 */

#include <stddef.h>
#include <stdint.h>

/* ---- 外设区基址（取值仅供桩内地址比较，与真实 stm32f407xx.h 一致） ------- */

#define APB1PERIPH_BASE ((uintptr_t) 0x40000000UL)
#define APB2PERIPH_BASE ((uintptr_t) 0x40010000UL)
#define AHB1PERIPH_BASE ((uintptr_t) 0x40020000UL)
#define AHB2PERIPH_BASE ((uintptr_t) 0x50000000UL)

/* ---- TIM 类型与通道宏 ---------------------------------------------------- */

typedef struct {
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
} TIM_TypeDef;

typedef struct {
    TIM_TypeDef *Instance;
} TIM_HandleTypeDef;

#define TIM_CHANNEL_1 0x00000000UL
#define TIM_CHANNEL_2 0x00000004UL
#define TIM_CHANNEL_3 0x00000008UL
#define TIM_CHANNEL_4 0x0000000CUL

#define IS_TIM_INSTANCE(x) ((x) != NULL)

#define __HAL_TIM_GET_AUTORELOAD(htim) ((htim)->Instance->ARR)

/* 桩按通道写入对应 CCRx，便于测试观察。真实 HAL 实现等价。 */
#define __HAL_TIM_SET_COMPARE(htim, ch, val)                       \
    do {                                                           \
        switch (ch) {                                              \
            case TIM_CHANNEL_1: (htim)->Instance->CCR1 = (val); break; \
            case TIM_CHANNEL_2: (htim)->Instance->CCR2 = (val); break; \
            case TIM_CHANNEL_3: (htim)->Instance->CCR3 = (val); break; \
            case TIM_CHANNEL_4: (htim)->Instance->CCR4 = (val); break; \
            default: break;                                        \
        }                                                          \
    } while (0)

/* ---- RCC 类型与函数 ------------------------------------------------------ */

typedef struct {
    uint32_t APB1CLKDivider;
    uint32_t APB2CLKDivider;
} RCC_ClkInitTypeDef;

#define RCC_HCLK_DIV1 0x00000000UL
#define RCC_HCLK_DIV2 0x00001000UL
#define RCC_HCLK_DIV4 0x00001400UL

/* 桩默认：APB 分频 == 1，PCLK1 = 42 MHz，PCLK2 = 84 MHz（接近 F407 典型配置）。
 * 测试需要其它分频时直接修改 RCC_ClkInitTypeDef 输出字段值。 */
static inline void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *cfg, uint32_t *latency) {
    if (cfg != NULL) {
        cfg->APB1CLKDivider = RCC_HCLK_DIV1;
        cfg->APB2CLKDivider = RCC_HCLK_DIV1;
    }
    if (latency != NULL) {
        *latency = 0;
    }
}

static inline uint32_t HAL_RCC_GetPCLK1Freq(void) {
    return 42000000UL;
}

static inline uint32_t HAL_RCC_GetPCLK2Freq(void) {
    return 84000000UL;
}
