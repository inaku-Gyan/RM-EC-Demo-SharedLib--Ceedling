#pragma once

/* ============================================================================
 *  库内部使用的配置入口（库代码统一从这里拿到所有 SL_* 宏）。
 *
 *  覆写机制：业务方在编译命令中提供 -DSL_USER_CONFIG=\"path/to/header.h\"，
 *  本文件最先 #include 该头让业务方先 #define 想覆盖的项；之后再 #include
 *  sl_config_default.h，由其内部的 #ifndef 守卫为未覆写的项填默认值。
 *
 *  仓库内单元测试默认不定义 SL_USER_CONFIG——mixin 通过 -DSL_USE_X=1 直接
 *  注入个别开关，未注入项由 sl_config_default.h 兜底。
 *
 *  关于 vendor 头：本文件不自动 #include 任何外部库头，库源文件自己用
 *      #if SL_USE_X
 *      #include SL_X_INCLUDE
 *      ...
 *      #endif
 *  按需引入。case 1（必须依赖）配 #else #error 拒绝错误配置。
 * ============================================================================ */

#ifdef SL_USER_CONFIG
    #include SL_USER_CONFIG
#endif

#include "sl_config_default.h"

/* 全局 sanity check。 */

#ifdef SL_USE_HAL
    #error "SL_USE_HAL 由 SL_USE_HAL_F4/H7 派生，不允许直接定义"
#else
    #define SL_USE_HAL (SL_USE_HAL_F4 + SL_USE_HAL_H7)
    #if SL_USE_HAL != 0 && SL_USE_HAL != 1
        #error "SL_USE_HAL_F4/H7 定义冲突"
    #endif
#endif

#if SL_USE_HAL == 0
    #undef SL_INCLUDE_HAL
#endif
