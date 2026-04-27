#pragma once

#include <stdint.h>

/* 测试期 CMSIS-DSP 桩头：暴露示例模块用到的最小函数面，签名与官方 CMSIS-DSP
 * 一致，真实集成时把 SL_INCLUDE_ARM_MATH 指向 vendor 头即可无缝替换。 */

static inline void arm_dot_prod_f32(const float *a, const float *b,
                                    uint32_t n, float *result) {
    float sum = 0.0f;
    for (uint32_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    *result = sum;
}
