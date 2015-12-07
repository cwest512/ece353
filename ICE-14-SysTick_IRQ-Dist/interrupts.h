#ifndef __ECE353_INTERRUPTS_H__
#define __ECE353_INTERRUPTS_H__

#include <stdint.h>
#include <stdbool.h>
#include "TM4C123.h"

extern volatile bool AlertSysTick;
extern volatile bool DisableTone;

void SysTick_Handler(void);

#endif
