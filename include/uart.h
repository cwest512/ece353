#ifndef __UART_H__
#define __UART_H__

#include "driver_defines.h"

//************************************************************************
// Configure UART0 to be 115200, 8N1.  Data will be sent/recieved using
// polling (Do Not enable interrupts)
//************************************************************************
bool uart_init_115K(
  uint32_t base_addr
);
bool verifyUartBase(uint32_t base);
void uartTxPollChar(uint32_t base, char data);
void uartTxPoll(uint32_t base, char *data);
char uartRxPoll(uint32_t base, bool block);
#endif
