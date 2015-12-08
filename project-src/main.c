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

char const write_data[] = "brad";
uint8_t read_data[4];


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

void read_eeprom(void)
{
	int i;
	for(i = 0; i < 4; i++)
  {
    eeprom_byte_read(EEPROM_I2C_BASE,i,&(read_data[i]));
  }
}

void write_eeprom(void)
{
	int i;
	for(i = 0; i < sizeof(write_data); i++)
  {
		eeprom_byte_write(EEPROM_I2C_BASE,i, write_data[i]);
	}
}

int 
main(void)
{	
	int i;
	char c;
	uint8_t data;
	uint32_t x_data, y_data;
	initializeBoard();
	printf("This is a test\n");
	write_eeprom();
	read_eeprom();
//	c = (char *) read_data;
	for(i = 0; i < sizeof(write_data); i++)
  {
		c = (char) read_data[i];
		printf("%c", c);
	}
  while(1)
  {
    data = GPIOF->DATA;
    
    if( (data & DIR_BTN_UP) == 0)
    {
      printf("BTN_UP pressed\n\r");
    }
    
    if( (data & DIR_BTN_DOWN) == 0)
    {
      printf("BTN_DOWN pressed\n\r");
    }
    
    if( (data & DIR_BTN_RIGHT) == 0)
    {
      printf("BTN_RIGHT pressed\n\r");
    }
    
    if( (data & DIR_BTN_LEFT) == 0)
    {
      printf("BTN_LEFT pressed\n\r");
    }
    
    if((GPIOE->DATA & PS2_BTN) == 0)
    {
			//Print EEprom
			//Allow for edit of eeprom
      printf("PS2_BTN pressed\n\r");
    }
    x_data = getADCValue(PS2_ADC_BASE,PS2_X_ADC_CHANNEL);
    y_data = getADCValue(PS2_ADC_BASE,PS2_Y_ADC_CHANNEL);
    //printf("X Dir value : 0x%03x        Y Dir value : 0x%03x\r",x_data, y_data);
		
		if(y_data > 0xc00)
				printf("y up");
		if(y_data < 0x400)
				printf("y down");
		if(x_data > 0xc00)
				printf("x up");
		if(x_data < 0x400)
				printf("x down");
    for(i=0;i<1000000; i++){}
  }
}
