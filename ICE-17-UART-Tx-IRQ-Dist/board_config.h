#ifndef __BOARDUTIL_H__
#define __BOARDUTIL_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "driver_defines.h"
#include "gpio_port.h"
#include "pc_buffer.h"
#include "uart_irqs.h"



extern PC_Buffer UART0_Tx_Buffer;
extern PC_Buffer UART0_Rx_Buffer;


// Function Prototypes
void initializeBoard(void);
void DisableInterrupts(void);
void EnableInterrupts(void);

#endif
