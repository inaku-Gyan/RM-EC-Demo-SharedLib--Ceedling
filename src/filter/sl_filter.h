#pragma once

#include <stdint.h>

/* Vector dot product. Implementation may use CMSIS-DSP when SL_USE_ARM_DSP=1. */
float sl_filter_dot(const float *a, const float *b, uint32_t n);
