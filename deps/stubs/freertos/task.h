#pragma once

/* Scaffold stub: critical-section macros are no-ops on the host so unit tests
 * can run. Real targets override SL_FREERTOS_TASK_INCLUDE to the vendor header. */

#define taskENTER_CRITICAL() do {} while (0)
#define taskEXIT_CRITICAL()  do {} while (0)
