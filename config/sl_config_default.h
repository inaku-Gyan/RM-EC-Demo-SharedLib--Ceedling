#pragma once

/* ============================================================================
 *  EC-Shared-Lib default configuration — user-tunable knobs only.
 *
 *  Downstream "sl_config.h" provided by the business repo may #define entries
 *  before #including this file to override; the #ifndef guards below preserve
 *  those overrides. Do not add anything here that is NOT meant for the user
 *  to set — internal/derived macros belong in sl_config_internal.h.
 *
 *  Library code MUST NOT include this header directly — go through
 *  sl_config_internal.h instead.
 * ============================================================================ */

/* ----- Feature switches: 0 = off, 1 = on ---------------------------------- */

#ifndef SL_USE_FREERTOS
#define SL_USE_FREERTOS 0
#endif

#ifndef SL_USE_ARM_DSP
#define SL_USE_ARM_DSP 0
#endif

#ifndef SL_USE_HAL
#define SL_USE_HAL 0
#endif

/* ----- HAL version selectors (only meaningful when SL_USE_HAL = 1) -------- */

#ifndef SL_HAL_VERSION_F4
#define SL_HAL_VERSION_F4 0
#endif

#ifndef SL_HAL_VERSION_H7
#define SL_HAL_VERSION_H7 0
#endif

/* ----- External dependency include paths ---------------------------------- *
 *  Override these to point at your vendor headers. Library code never
 *  references SL_*_INCLUDE directly — sl_config_internal.h consumes them.
 * -------------------------------------------------------------------------- */

#ifndef SL_FREERTOS_INCLUDE
#define SL_FREERTOS_INCLUDE      "FreeRTOS.h"
#endif

#ifndef SL_FREERTOS_TASK_INCLUDE
#define SL_FREERTOS_TASK_INCLUDE "task.h"
#endif

#ifndef SL_ARM_MATH_INCLUDE
#define SL_ARM_MATH_INCLUDE      "arm_math.h"
#endif

#ifndef SL_HAL_INCLUDE
#define SL_HAL_INCLUDE           "stm32f4xx_hal.h"
#endif
