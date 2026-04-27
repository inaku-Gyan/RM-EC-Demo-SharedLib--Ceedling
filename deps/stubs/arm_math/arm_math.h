#pragma once

#include <stdint.h>

/* Scaffold stub: minimal CMSIS-DSP surface needed by example modules. Mirrors
 * the real CMSIS-DSP signature so library code compiles unchanged when this
 * is swapped for the vendor header. */

static inline void arm_dot_prod_f32(const float *a, const float *b,
                                    uint32_t n, float *result) {
    float sum = 0.0f;
    for (uint32_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    *result = sum;
}
