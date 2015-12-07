#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver_defines.h"


//*****************************************************************************
// Configure a 16/32 bit general purpose timer to wait a specified number
// of clock cycles
//*****************************************************************************
bool gp_timer_wait(uint32_t base, uint32_t ticks);


#endif
