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

#define TX_MODE	true

/******************************************************************************
 * Global Variables
 *****************************************************************************/
// ADD CODE
uint8_t myID[]      = { '3', '5', '3', '0', '4'};
uint8_t remoteID[]  = { '3', '5', '3', '4', '4'};


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
	//Initializes SPI interface and LCD
	//lcd_init();	
	rfInit();
	
  EnableInterrupts();
}

int 
main(void)
{	
	wireless_com_status_t status;
  uint32_t data;
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

  // Infinite Loop
  while(1)
  {

      if(TX_MODE && AlertOneSec)
      {
          printf("Sending: %d\n\r",i);
          status = wireless_send_32(false, false, i);
          AlertOneSec = false;
          i++;
      }
      else if (!TX_MODE)
      {
        status =  wireless_get_32(false, &data);
        if(status == NRF24L01_RX_SUCCESS)
        {
            printf("Received: %d\n\r", data);
        }
        
        AlertOneSec = false;
      }
    }
}
