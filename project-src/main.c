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
#include "wireless.h"
#include "timers.h"
#include "interrupts.h"

#define TX_MODE	true

/******************************************************************************
 * Global Variables
 *****************************************************************************/
// ADD CODE
uint8_t myID[]      = { '3', '5', '3', '0', '4'};
uint8_t remoteID[]  = { '3', '5', '3', '4', '4'};
uint32_t data = 0xFFFF;
uint32_t ps2data = 0xFFFF;
uint32_t upcounter = 0;
uint32_t downcounter = 0;
uint32_t leftcounter = 0;
uint32_t rightcounter = 0;
uint32_t ps2counter = 0;
uint32_t x_data, y_data;

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
	SysTick_Config(25000);
	//Initializes SPI interface and LCD
	//lcd_init();	
	rfInit();
	
  EnableInterrupts();
}

int 
main(void)
{	
	wireless_com_status_t status;
  //uint32_t data;
  int i = 0;
	
	initializeBoard();
	  printf("\n\r");
  printf("**************************************\n\r");
  printf("* ECE353 - SPI ICE\n\r");
  printf("**************************************\n\r");
  printf("\n\r");
  
  printf("MyID:%c%c%c%c%c\n\r",myID[0],myID[1],myID[2],myID[3],myID[4]);
  printf("RmID:%c%c%c%c%c\n\r",remoteID[0],remoteID[1],remoteID[2],remoteID[3],remoteID[4]);
  
  wireless_configure_device(myID, remoteID ) ;
  
  printf("\n\r");
	
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
  // Infinite Loop
  while(1)
  {

//      if(TX_MODE && AlertOneSec)
//      {
//          printf("Sending: %d\n\r",i);
//          status = wireless_send_32(false, false, i);
//          AlertOneSec = false;
//          i++;
//      }
//      else if (!TX_MODE)
//      {
//        status =  wireless_get_32(false, &data);
//        if(status == NRF24L01_RX_SUCCESS)
//        {
//            printf("Received: %d\n\r", data);
//        }
//        
//        AlertOneSec = false;
//      }
		
		if(AlertSysTick)
		{
			data = GPIOF->DATA;
			ps2data = GPIOE->DATA;
		}
		
		if((data & DIR_BTN_UP) == 0)
		{
			upcounter++;
			if(upcounter >= 4)
			{
				printf("up is pressed\n");
			}
		}
		else {
			upcounter = 0;
		}
		
		if( (data & DIR_BTN_DOWN) == 0)
		{
			downcounter++;
			if(downcounter >= 4)
			{
				printf("down is pressed\n");
			}
		}
		else {
			downcounter = 0;
		}
		
		if( (data & DIR_BTN_LEFT) == 0)
		{
			leftcounter++;
			if(leftcounter >= 4)
			{
				printf("left is pressed\n");
			}
		}
		else {
			leftcounter = 0;
		}
		
		if( (data & DIR_BTN_RIGHT) == 0)
		{
			rightcounter++;
			if(rightcounter >= 4)
			{
				printf("right is pressed\n");
			}
		}
		else {
			rightcounter = 0;
		}
		
		if( ((ps2data & PS2_BTN) == 0))
		{
			ps2counter++;
			if(ps2counter >= 4)
			{
				printf("ps2 is pressed\n");
			}
		}
		else {
			ps2counter = 0;
		}
		if(updateXY)
		{
				x_data = getADCValue(PS2_ADC_BASE,PS2_X_ADC_CHANNEL);
				y_data = getADCValue(PS2_ADC_BASE,PS2_Y_ADC_CHANNEL);
				//printf("X Dir value : 0x%03x        Y Dir value : 0x%03x\r",x_data, y_data);
				if(y_data > 0xc00)
					printf("y up\n");
				if(y_data < 0x400)
					printf("y down\n");
				if(x_data > 0xc00)
						printf("x up\n");
				if(x_data < 0x400)
						printf("x down\n");
				updateXY = false;
		}
//			  memset(input,0,80);
//				printf("\n\rEnter a string: ");
//				scanf("%79[^\n]", input);
//				printf("You entered: %s\n\r",input);
    }
}
