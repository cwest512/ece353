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
uint8_t myID[]      = { '3', '5', '3', '0', '4'};
uint8_t remoteID[]  = { '3', '5', '3', '1', '3'};


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
	//Initializes SPI interface and LCD
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
  int i = 0;
	uint16_t *pressed = (uint16_t *) malloc(sizeof(uint16_t));
	
	initializeBoard();
	  printf("\n\r");
  printf("**************************************\n\r");
  printf("* ECE353 - SPI ICE\n\r");
  printf("**************************************\n\r");
  printf("\n\r");
  
  printf("MyID:%c%c%c%c%c\n\r",myID[0],myID[1],myID[2],myID[3],myID[4]);
  printf("RmID:%c%c%c%c%c\n\r",remoteID[0],remoteID[1],remoteID[2],remoteID[3],remoteID[4]);
  
  wireless_configure_device(myID, remoteID ) ;
  buttonToBePressed = random_generate();
  printf("\n\r");
	printf("Displaying image...\n");
	
	*pressed = 0;
	if(TX_MODE)
	{
		printf("Tx Mode\n\r");
	}
	else
	{
		printf("Rx Mode\n\r");
	}
	
	//turn on timer0A
	a_timer->CTL |= TIMER_CTL_TAEN;
	one_timer->CTL |= TIMER_CTL_TAEN;
  // Infinite Loop
  while(1)
  {

//      if(TX_MODE && AlertOneSec)
//      {
//				printf("test wireless_send");
////          printf("Sending: %d\n\r",i);
////          status = wireless_send_32(false, false, i);
////          AlertOneSec = false;
////          i++;
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
//      }



		for(i = 0; i < 10000000; i++)
		{
			
			read_buttons(pressed);
			if( buttonToBePressed == *pressed)
			{
				correct = true;
				i=10000000;
			}
			else if ( (~buttonToBePressed & *pressed) != 0 )
			{
				i=10000000;
				correct = false;		
			}
		}
		if(correct)
		{
			buttonToBePressed = random_generate();
			printf("correct");
		}
		else
		{
			printf("wrong");
			buttonToBePressed = random_generate();
		}
		while(initState);
		*pressed = 0;
		
		petTheDog(500E6);

	}
}