#pragma once

/* ============================================================================
 *  EC-Shared-Lib 默认配置——只放「用户可调」的开关与路径，不放任何
 *  库内部派生的宏。
 *
 *  业务方在编译时通过 -DSL_USER_CONFIG=\"path/to/header.h\" 指定自己的
 *  配置头，对感兴趣的项做 #define；src/config/internal.h 会先 #include
 *  业务配置、再 #include 本文件，下方每条 #ifndef 守卫保留业务方覆写。
 *
 *  库代码绝不直接 #include 本文件——必须经由 src/config/internal.h 间接拿到。
 * ============================================================================
 */

/* ----- 断言 ------------------------------------------------------------- *
 *  库内部统一走 SL_ASSERT(expr)。默认转发到 <assert.h> 的 assert()——
 *  host 单元测试零配置可用，业务方定义 NDEBUG 即可整段擦除。
 *
 *  嵌入式现场不愿走 newlib 的 __assert_func / stderr 时，业务可在自己的
 *  SL_USER_CONFIG 头里整段覆写：
 *      #define SL_ASSERT(expr) ((expr) ? (void)0 : my_panic(__FILE__, __LINE__))
 *  release 期希望零开销也可直接：
 *      #define SL_ASSERT(expr) ((void)0)
 *  注意覆写后本文件不再 #include <assert.h>。
 * -------------------------------------------------------------------------- */

#ifndef SL_ASSERT
    #include <assert.h> // IWYU pragma: keep
    #define SL_ASSERT(expr) assert(expr)
#endif

/* ----- 功能开关：0 = 关闭，1 = 启用 -------------------------------------- */

#ifndef SL_USE_FREERTOS
    #define SL_USE_FREERTOS 0
#endif

/* ----- HAL 版本选择 ------------------ */

#ifndef SL_USE_HAL_F4
    #define SL_USE_HAL_F4 0
#endif

#ifndef SL_USE_HAL_H7
    #define SL_USE_HAL_H7 0
#endif

/* ----- 外部依赖头文件路径 ------------------------------------------------- *
 *  让业务方告诉库去哪里 #include vendor 头。
 * -------------------------------------------------------------------------- */

#if SL_USE_FREERTOS

    #ifndef SL_INCLUDE_FREERTOS
        #define SL_INCLUDE_FREERTOS "FreeRTOS.h"
    #endif

    #ifndef SL_INCLUDE_FREERTOS_TASK
        #define SL_INCLUDE_FREERTOS_TASK "task.h"
    #endif

#else // 关闭 FreeRTOS 时不应定义相关路径

    #ifdef SL_INCLUDE_FREERTOS
        #error "SL_USE_FREERTOS = 0 时不应定义 SL_INCLUDE_FREERTOS"
    #endif

    #ifdef SL_INCLUDE_FREERTOS_TASK
        #error "SL_USE_FREERTOS = 0 时不应定义 SL_INCLUDE_FREERTOS_TASK"
    #endif

#endif // SL_USE_FREERTOS

#ifndef SL_INCLUDE_HAL
    #define SL_INCLUDE_HAL "stm32f4xx_hal.h"
#endif
