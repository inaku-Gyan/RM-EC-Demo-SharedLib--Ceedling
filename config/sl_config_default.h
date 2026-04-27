#pragma once

/* ============================================================================
 *  EC-Shared-Lib 默认配置——只放「用户可调」的开关与路径，不放任何
 *  库内部派生的宏。
 *
 *  业务方在编译时通过 -DSL_USER_CONFIG=\"path/to/header.h\" 指定自己的
 *  配置头，对感兴趣的项做 #define；config/internal.h 会先 #include 业务
 *  配置、再 #include 本文件，下方每条 #ifndef 守卫保留业务方覆写。
 *
 *  库代码绝不直接 #include 本文件——必须经由 config/internal.h 间接拿到。
 * ============================================================================ */

/* ----- 功能开关：0 = 关闭，1 = 启用 -------------------------------------- */

#ifndef SL_USE_FREERTOS
#define SL_USE_FREERTOS 0
#endif

#ifndef SL_USE_ARM_DSP
#define SL_USE_ARM_DSP 0
#endif

#ifndef SL_USE_HAL
#define SL_USE_HAL 0
#endif

/* ----- HAL 版本选择（仅当 SL_USE_HAL=1 时有意义） -------------------------- */

#ifndef SL_HAL_VERSION_F4
#define SL_HAL_VERSION_F4 0
#endif

#ifndef SL_HAL_VERSION_H7
#define SL_HAL_VERSION_H7 0
#endif

/* ----- 外部依赖头文件路径 ------------------------------------------------- *
 *  让业务方告诉库去哪里 #include vendor 头。库代码绝不直接引用 SL_*_INCLUDE，
 *  由 try_*.h / require_*.h 在 #if 里使用。
 * -------------------------------------------------------------------------- */

#ifndef SL_INCLUDE_FREERTOS
#define SL_INCLUDE_FREERTOS      "FreeRTOS.h"
#endif

#ifndef SL_INCLUDE_FREERTOS_TASK
#define SL_INCLUDE_FREERTOS_TASK "task.h"
#endif

#ifndef SL_INCLUDE_ARM_MATH
#define SL_INCLUDE_ARM_MATH      "arm_math.h"
#endif

#ifndef SL_INCLUDE_HAL
#define SL_INCLUDE_HAL           "stm32f4xx_hal.h"
#endif
