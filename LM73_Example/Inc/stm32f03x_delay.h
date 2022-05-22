#ifndef DELAY_H
#define DELAY_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_defs.h"
// ---------------------------------------------------------------------------
void Delay_Init(void);
void Delay500ns(uint32_t time_500ns);
void DelayUs(uint32_t us);
void DelayMs(uint32_t ms);
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif
