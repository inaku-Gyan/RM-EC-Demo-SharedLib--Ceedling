#pragma once

#include <stdint.h>

/* 向量点积。当 SL_USE_ARM_DSP=1 时实现走 CMSIS-DSP 加速路径。 */
float sl_filter_dot(const float *a, const float *b, uint32_t n);
