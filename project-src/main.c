//*****************************************************************************
// main.c
// Author:
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "TM4C123.h"
#include "eeprom.h"
#include "board_util.h"
#include "uart_irqs.h"
#include "uart.h"
#include "gpio_port.h"
#include "adc.h"
#include "wireless.h"
#include "timers.h"
#include "interrupts.h"
#include "lcd.h"
#include "gamelogic.h"

#define TX_MODE	true
#define BTN_UP 0x1
#define BTN_DOWN 0x2
#define BTN_LEFT 0x4
#define BTN_RIGHT 0x8
#define BTN_PS2 0x10
#define PS2_UP 0x20
#define PS2_DOWN 0x40
#define PS2_LEFT 0x80
#define PS2_RIGHT 0x100



/******************************************************************************
 * Global Variables
 *****************************************************************************/
// ADD CODE
uint8_t myID[]      = { '3', '5', '3', '1', '3'};
uint8_t remoteID[]  = { '3', '5', '3', '0', '4'};


//*****************************************************************************
//*****************************************************************************
void initializeBoard(void)
{
  DisableInterrupts();
	
	adc_init();
	pushButton_init();
	uart0_config_gpio();
  initialize_uart();
	i2cInit();
	timer0_configA(25000);
	timer1_configA(250E6);
	watchdog_config(500E6);
	SysTick_Config(25000);
	srand(5);
	wireless_configure_device(myID, remoteID );
	lcd_init();	
	rfInit();
	
  EnableInterrupts();
}


int 
main(void)
{	
	wireless_com_status_t status;
	int buttonToBePressed;
	bool correct = true;
	bool wait;
  int counter;
	int score = 0;
	uint16_t *pressed = (uint16_t *) malloc(sizeof(uint16_t));

	initializeBoard();
  buttonToBePressed = random_generate();
	
//	for(i = 0; i < 80; i++)
//		eeprom_byte_write(EEPROM_I2C_BASE, i, firstName[i]);
//	for(i=0;i<80;i++)
//		eeprom_byte_read(EEPROM_I2C_BASE, i, &toread[i] );
//		
//	for(i = 0; i < 80; i++)
//		eeprom_byte_write(EEPROM_I2C_BASE, i+80, secondName[i]);
//	for(i=0;i<80;i++)
//		eeprom_byte_read(EEPROM_I2C_BASE, i+80, &secondtoread[i] );
//	

//	printf("\nStudent 2: ");
//	for(i=0;i<80;i++)
//		printf("%c", secondName[i]);

	a_timer->CTL |= TIMER_CTL_TAEN;
	one_timer->CTL |= TIMER_CTL_TAEN;
	
	
  while(1)
  {

//      if(TX_MODE && AlertOneSec)
//      {
//          printf("Sending: %d\n\r",i);
//          status = wireless_send_32(false, false, '1020');
//          AlertOneSec = false;
//          i++;
//				AlertOneSec = false;
//      }
//      else if (!TX_MODE)
//      {
//        status =  wireless_get_32(false, &data);
//        if(status == NRF24L01_RX_SUCCESS)
//        {
//            printf("Received: %d\n\r", data);
//						
//        }
//        
//        AlertOneSec = false;
 //     }


		counter = 0;
		*pressed = 0;
		wait = true;
		
		while(wait)
		{
			read_buttons(pressed);
			if( buttonToBePressed == *pressed)
			{
				wait = false;
				correct = true;
			}
			else if ( ((~buttonToBePressed & *pressed) != 0 ) || counter > 1000000 )
			{
				wait = false;
				correct = false;		
			}
			counter++;
		}
		
		if(correct)
		{
			score++;
			if(score == 50)
			 endGame(true, score);
		}
		else
		{
			endGame(false, score);
		}
		
		while(!initState)
		{
			read_buttons(pressed);
		}
		
		buttonToBePressed = random_generate();
		
		

	}
}
