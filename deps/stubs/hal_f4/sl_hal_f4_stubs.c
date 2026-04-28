/*
 * STM32F4 HAL 测试桩——仅在 host 单元测试中链接进来。
 *
 * 真实 vendor `.c`（stm32f4xx_hal_rcc.c 等）链路上会拉入 cmsis_gcc.h 中的
 * ARM Cortex-M 内联汇编（isb/dmb/wfe 等），host gcc 无法编译，所以 mixin
 * 不把 deps/.../Src 放入 :source:。但 sl_hal_ext 模块（如 tim.c）调用了
 * HAL_RCC 等函数，链接时需要这些符号；本文件提供 host 端最小桩。
 *
 * 桩只关心“能链接”，不模拟时钟系统真实行为。需要在测试中验证具体频率值时，
 * 改用 CMock 自动生成 mock，或为该测试场景手写返回特定值的局部桩——本文件
 * 不应膨胀成行为模拟。
 */

#include "stm32f4xx_hal.h"

void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency) {
    if (RCC_ClkInitStruct != NULL) {
        *RCC_ClkInitStruct = (RCC_ClkInitTypeDef){0};
    }
    if (pFLatency != NULL) {
        *pFLatency = 0;
    }
}

uint32_t HAL_RCC_GetPCLK1Freq(void) {
    return 0;
}

uint32_t HAL_RCC_GetPCLK2Freq(void) {
    return 0;
}
