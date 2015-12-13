#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver_defines.h"

extern volatile TIMER0_Type *a_timer;
extern volatile TIMER0_Type *one_timer;

//*****************************************************************************
// Configure a 16/32 bit general purpose timer to wait a specified number
// of clock cycles
//*****************************************************************************
bool gp_timer_wait(uint32_t base, uint32_t ticks);
void timer0_configA(uint16_t ticks);
void timer1_configA(uint32_t ticks);
void watchdog_config(uint32_t ticks);
void petTheDog(uint32_t ticks);

#endif
