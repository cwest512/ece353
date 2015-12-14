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

#define TX_MODE	false
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
uint8_t myID[]      = { '3', '5', '3', '6', '5'};
uint8_t remoteID[]  = { '3', '5', '3', '6', '4'};


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
	
	rfInit();
	gpioD_enable_interrupt();
	wireless_configure_device(myID, remoteID );
	
	
	lcd_init();	
	
	
  EnableInterrupts();
}


int 
main(void)
{	
	wireless_com_status_t status;
	int buttonToBePressed;
	uint32_t recived;
	uint32_t droppedPacket = 0;
	uint32_t sentPacket = 0;
	uint32_t recievedPacket = 0;
	bool correct = true;
	bool wait;
  int counter;
	int score = 0;
	int j = 0;
	uint8_t toRead[80];
	uint16_t *pressed = (uint16_t *) malloc(sizeof(uint16_t));

	initializeBoard();
  buttonToBePressed = random_generate();
	
	printf("\nStudent 1: ");
	
	for(j = 0; j < 80; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j, &toRead[j] );
		
	for(j = 0; j < 80; j++)
		printf("%c", toRead[j]);
	
	printf("\nStudent 2: ");
	
	for(j = 0; j < 80; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j+80, &toRead[j] );
	
	for(j = 0; j < 80; j++)
		printf("%c", toRead[j]);
		
	printf("\nTeam Number: ");
	
	for(j = 0; j < 2; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j+160, &toRead[j] );

	for(j = 0; j < 2; j++)
		printf("%c", toRead[j]);
	
	a_timer->CTL |= TIMER_CTL_TAEN;
	one_timer->CTL |= TIMER_CTL_TAEN;
	
//	while(1)
//	{
//		if(readIn)
//		{
//		status = wireless_get_32(false, &recived);
//        if(status == NRF24L01_RX_SUCCESS)
//          printf("Received: %d\n\r", recived);
//				readIn = false;
//			}
//	}
  while(1)
  {

	if(TX_MODE)
	{
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
			petTheDog(500E6);
			
			//if(status == NRF24L01_TX_PCK_LOST)
			//{
			//	while(true);
			//}
				
//			if(readIn)
//			{
//				status = wireless_get_32(false, &recived);
//        if(status == NRF24L01_RX_SUCCESS)
//        {
//            printf("Received: %d\n\r", recived);
//						recievedPacket++;					
//        }
//					
//				readIn = false;
//			}
//			else
//				status = wireless_send_32(false, false, score);
//				if(status == NRF24L01_TX_PCK_LOST)
//					droppedPacket++;
//				if(status == NRF24L01_TX_SUCCESS)
//					sentPacket++;
		}
		
		if(AlertFiveSec)
		{
			AlertFiveSec = false;
			
			printf("Number of packets dropped: %d\nNumber of packets sent: %d\n",droppedPacket, sentPacket);
			printf("Number of packets recieved: %d\n", recievedPacket);
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
	else
	{
		if(readIn)
			{
				status = wireless_get_32(false, &recived);
        if(status == NRF24L01_RX_SUCCESS)
        {
            printf("Received: %d\n\r", recived);
						recievedPacket++;					
        }
				else
					droppedPacket++;
				readIn = false;
			}
		if(notReadIn)
		{
			status = wireless_send_32(false, false, score);
			printf("sending..\n");
			notReadIn = false;
		}
			
		notReadIn = true;
			
		if(AlertFiveSec)
		{
			AlertFiveSec = false;
			
			printf("Number of packets dropped: %d\nNumber of packets sent: %d\n",droppedPacket, sentPacket);
			printf("Number of packets recieved: %d\n", recievedPacket);
		}
	}
	
	}
}
