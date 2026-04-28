#include "../../../config/internal.h"
#if SL_USE_HAL

    #include "unity.h"

    #include "../tim.h"
TEST_SOURCE_FILE("../tim.c")

// tim.c 调用 HAL_RCC_*；mixin 不挂 vendor .c（cortex/cmsis_gcc.h 等含 ARM
// 内联汇编无法在 host 编译），故走 deps/stubs/hal_f4 提供的 host 端桩。
TEST_SOURCE_FILE("sl_hal_f4_stubs.c")

void setUp(void) {
}
void tearDown(void) {
}

#endif // SL_USE_HAL
