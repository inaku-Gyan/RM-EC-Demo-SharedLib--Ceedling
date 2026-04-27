#pragma once

/* ============================================================================
 *  Library-internal configuration wrapper.
 *
 *  Library code (.c and any .h with #if SL_USE_* / dep API references) MUST
 *  include this header instead of sl_config_default.h or the bare sl_config.h.
 *  This header:
 *    - resolves the user-facing knobs (via sl_config.h → sl_config_default.h),
 *    - auto-#includes enabled vendor headers so library files never repeat
 *      `#include SL_*_INCLUDE` themselves,
 *    - declares derived/sanity macros (e.g., HAL version sanity check).
 *
 *  Two canonical patterns for files that touch external deps:
 *
 *  (1) File *requires* the dep — no fallback path:
 *
 *        #include "sl_config_internal.h"
 *        #if !SL_USE_FREERTOS
 *        #error "sl_motor_dji_rtos.c requires SL_USE_FREERTOS=1"
 *        #endif
 *
 *      Such files must NOT be unconditionally pulled into the build under
 *      mixins where the dep is off. For Ceedling tests, gate the
 *      TEST_SOURCE_FILE("...") directive on SL_USE_<DEP> in the test file.
 *
 *  (2) File *adapts* — has a fallback when the dep is absent:
 *
 *        #include "sl_config_internal.h"
 *        #if SL_USE_ARM_DSP
 *        arm_dot_prod_f32(...);
 *        #else
 *        for (uint32_t i = 0; i < n; ++i) sum += a[i] * b[i];
 *        #endif
 * ============================================================================ */

#include "sl_config.h" // IWYU pragma: export

/* ----- Auto-include enabled dependencies ---------------------------------- *
 *  The IWYU pragmas below tell include-cleaner that these headers are part
 *  of the wrapper's public surface — library files use vendor symbols (e.g.,
 *  taskENTER_CRITICAL) without including the vendor header themselves.
 * -------------------------------------------------------------------------- */

#if SL_USE_FREERTOS
#include SL_FREERTOS_INCLUDE      // IWYU pragma: export
#include SL_FREERTOS_TASK_INCLUDE // IWYU pragma: export
#endif

#if SL_USE_ARM_DSP
#include SL_ARM_MATH_INCLUDE      // IWYU pragma: export
#endif

#if SL_USE_HAL
#include SL_HAL_INCLUDE           // IWYU pragma: export
#endif

/* ----- Sanity checks ------------------------------------------------------ *
 *  Catch misconfigurations at compile time so they do not hide as silent
 *  fall-throughs. Add new checks here as new dep flags are introduced.
 * -------------------------------------------------------------------------- */

#if SL_USE_HAL && (SL_HAL_VERSION_F4 + SL_HAL_VERSION_H7) != 1
#error "SL_USE_HAL=1 requires exactly one of SL_HAL_VERSION_F4 / SL_HAL_VERSION_H7 to be set to 1"
#endif
