#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver_defines.h"

extern volatile TIMER0_Type *a_timer;
extern volatile TIMER0_Type *one_timer;
extern volatile TIMER0_Type *two_timer;
extern volatile WATCHDOG0_Type *wd_timer;

//*****************************************************************************
// Configure a 16/32 bit general purpose timer to wait a specified number
// of clock cycles
//*****************************************************************************
bool gp_timer_wait(uint32_t base, uint32_t ticks);

//********************************************************************************
// Configure Timer0A as a 32-bit, periodic, count-down timer and enable interrupts
// Paramters:
//    ticks: The number of clock cycles to wait.
void timer0_configA(uint16_t ticks);

//********************************************************************************
// Configure Timer1A as a 32-bit, periodic, count-down timer and enable interrupts
// Paramters:
//    ticks: The number of clock cycles to wait.
//********************************************************************************
void timer1_configA(uint32_t ticks);

//********************************************************************************
// Configure the Watchdog timer and generate interrupts
// Paramters:
//    ticks: The number of clock cycles to wait.
//********************************************************************************
void watchdog_config(uint32_t ticks);

//********************************************************************************
// Sets LOAD register as to not hault the system
// Paramters:
//    ticks: The number of clock cycles to wait.
//********************************************************************************
void petTheDog(uint32_t ticks);

//********************************************************************************
// Configure Timer2A as a 32-bit, periodic, count-down timer and enable interrupts
// Paramters:
//    ticks: The number of clock cycles to wait.
//********************************************************************************
void timer2_configA(uint32_t ticks);

#endif
