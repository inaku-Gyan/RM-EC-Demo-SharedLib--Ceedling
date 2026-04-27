#pragma once

/* 测试期 FreeRTOS 桩头：临界区宏在 host 上是 no-op，让单元测试可链接执行。
 * 真实目标会把 SL_INCLUDE_FREERTOS_TASK 指向 vendor 的 task.h。 */

#define taskENTER_CRITICAL() do {} while (0)
#define taskEXIT_CRITICAL()  do {} while (0)
