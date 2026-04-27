#include "../../config/internal.h"
#include "sl_filter.h"

#if SL_USE_ARM_DSP
#include SL_INCLUDE_ARM_MATH
#endif

float sl_filter_dot(const float *a, const float *b, uint32_t n) {
#if SL_USE_ARM_DSP
    float result;
    arm_dot_prod_f32(a, b, n, &result);
    return result;
#else
    float result = 0.0f;
    for (uint32_t i = 0; i < n; ++i) {
        result += a[i] * b[i];
    }
    return result;
#endif
}
