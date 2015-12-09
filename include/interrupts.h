#ifndef __ECE353_INTERRUPTS_H__
#define __ECE353_INTERRUPTS_H__

#include <stdint.h>
#include <stdbool.h>
#include "TM4C123.h"
#include "pc_buffer.h"
#include "driver_defines.h"

extern volatile bool AlertSysTick;
extern volatile bool AlertOneSec;
extern PC_Buffer UART0_Rx_Buffer;
extern PC_Buffer UART0_Tx_Buffer;
extern volatile bool updateXY;

__INLINE static void UART0_Rx_Flow(PC_Buffer *rx_buffer);
__INLINE static void UART0_Tx_Flow(PC_Buffer *tx_buffer);
void UART0_Handler(void);
void SysTick_Handler(void);

#endif
