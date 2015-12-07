//*****************************************************************************
// main.c
// Author:
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "TM4C123.h"
#include "eeprom.h"
#include "board_util.h"
#include "uart_irqs.h"
#include "uart.h"
#include "gpio_port.h"

//*****************************************************************************
//*****************************************************************************
void uart0_config_gpio(void)
{
	gpio_enable_port(GPIOA_BASE);
	gpio_config_digital_enable(GPIOA_BASE, 0x3);
	gpio_config_alternate_function(GPIOA_BASE, 0x3);
	GPIOA->PCTL |= (GPIO_PCTL_PA1_U0TX|GPIO_PCTL_PA0_U0RX);
}

void initializeBoard(void)
{
  DisableInterrupts();
	
	uart0_config_gpio();
  initialize_uart();
//  i2cInit();
	
  EnableInterrupts();
}

//void read_eeprom(void)
//{
//	int i;
//	uint8_t read_data[1];
//	for(i = 0; i < 1; i++)
//  {
//    eeprom_byte_read(EEPROM_I2C_BASE,i,&(read_data[i]));
//  }
//}

int 
main(void)
{	
	initializeBoard();
	printf("\na");
	
  while(1)
  {
  }
}
