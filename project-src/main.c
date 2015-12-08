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
#include "adc.h"

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
	
	adc_init();
	pushButton_init();
	uart0_config_gpio();
  initialize_uart();
	i2cInit();
	//Initializes SPI interface and LCD
	//lcd_initialize();
	
	//wireless init
	
  EnableInterrupts();
}



int 
main(void)
{	
	int i;
	uint8_t data;
	uint32_t x_data, y_data;
	
	initializeBoard();
	

  while(1)
  {
  }
}
