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
	uint8_t pins = PA0 | PA1;
	uint8_t out = PA1;
	uint8_t in = PA0;
	
	if(!gpio_enable_port(GPIOA_BASE))
	{
		while(1){}
	}
	
	if(!gpio_config_digital_enable(GPIOA_BASE,pins))
	{
		while(1){}
	}
	
	if(!gpio_config_alternate_function(GPIOA_BASE,pins))
	{
		while(1){}
	}
	
	if(!gpio_config_uart(GPIOA_BASE))
	{
		while(1){}
	}
}

void initializeBoard(void)
{
  DisableInterrupts();
	
	uart0_config_gpio();
  initialize_uart();
	
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
	printf("This is a test\n");
	
  while(1)
  {
		printf("This is a test\n");
  }
}
