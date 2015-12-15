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
//My Wireless id and partners wireless id
uint8_t myID[]      = { '3', '5', '3', '6', '4'};
uint8_t remoteID[]  = { '3', '5', '3', '6', '5'};


//*****************************************************************************
//*****************************************************************************
void initializeBoard(void)
{
	//Disable Interupts
  DisableInterrupts();
	//Intialize ADC
	adc_init();
	//Init Pushbuttons
	pushButton_init();
	//Initialize Uart0
	uart0_config_gpio();
  initialize_uart();
	//Init i2C
	i2cInit();
	//Init timers 0A, 1A and 2A and Systick with specific times
	timer0_configA(25000);
	timer2_configA(50E6);
	timer1_configA(250E6);
	SysTick_Config(25000);
	//seed rand()
	srand(5);
	//Init radio
	rfInit();	
	//init lcd
	lcd_init();	
	//Enable interrupts
  EnableInterrupts();
}


int 
main(void)
{	
	//Wireless Status
	wireless_com_status_t status;
	//Button to be pressed
	int buttonToBePressed;
	//Packet trackers and the recieved value from other board
	uint32_t recived, droppedPacket, sentPacket, recievedPacket = 0;
	//speed game runs at
	uint32_t speed = 500000;
	//Boolean for correct answer, busy wait
	bool correct, wait = true;
	//transmit mode bool
	bool TX_MODE;
	//score value, increment value, and counter value for speed
	int score, j, counter = 0;
	//toRead from eeprom
	uint8_t toRead[80];
	//pointer for button that is pressed to pass to toher functions
	uint16_t *pressed = (uint16_t *) malloc(sizeof(uint16_t));

	//Init board
	initializeBoard();
  
	//Print intial print out
	//Student 1
	printf("\nStudent 1: ");
	
	//read student 1 from eeprom
	for(j = 0; j < 80; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j, &toRead[j] );
	
	//print out
	for(j = 0; j < 80; j++)
		printf("%c", toRead[j]);
	
	//Student 2
	printf("\nStudent 2: ");
	
	//Read from eeprom
	for(j = 0; j < 80; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j+80, &toRead[j] );
	
	//print out
	for(j = 0; j < 80; j++)
		printf("%c", toRead[j]);
	
	//Team Number
	printf("\nTeam Number: ");
	
	//read from eeprom
	for(j = 0; j < 2; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j+160, &toRead[j] );

	//print out
	for(j = 0; j < 2; j++)
		printf("%c", toRead[j]);
	
	//new line for looks
	printf("\n");
	
	//start all the timers
	a_timer->CTL |= TIMER_CTL_TAEN;
	two_timer->CTL |= TIMER_CTL_TAEN;
	one_timer->CTL |= TIMER_CTL_TAEN;

//Ask for TX_MODE of board
TX_MODE = mode_selector();

//if transmit then..
if(TX_MODE)
{
	//print instructions
	instructionsGame();
	//generate a new image and button to be pressed
	buttonToBePressed = random_generate();
}
//If recieve mode
else
{
	//print out instructions
	instructionsRecv();
}
	//Start watchdog
	watchdog_config(500E6);
	//Start interupts for external wireless pin
	gpioD_enable_interrupt();
	//config wireless
	wireless_configure_device(myID, remoteID );

	//Infinite loop
  while(1)
  {
	
	//if transmit mode
	if(TX_MODE)
	{
		//Wait for joystick and buttons to be returned to inital state
		while(!initState)
		{
			read_buttons(pressed);
		}
		
		//reset all varriables
		counter = 0;
		*pressed = 0;
		wait = true;
		
		//Wait for an answer to be given, whether its correct or not
		while(wait)
		{
			//read in the buttons to pressed
			read_buttons(pressed);
			//if right then set correct to true and leave loop
			if( buttonToBePressed == *pressed)
			{
				wait = false;
				correct = true;
			}
			//if not correct or the counter has surpased the total speed then correct is false and leave loop
			else if ( ((~buttonToBePressed & *pressed) != 0 ) || counter > speed )
			{
				wait = false;
				correct = false;		
			}
			//increment counter
			counter++;
		
			//if recieve wireless interrupt
		if(readIn)
			{
				//Get the package
				status = wireless_get_32(false, &recived);
				//if sucessful
        if(status == NRF24L01_RX_SUCCESS)
        {
						//print recieved package for debugging
            //printf("Received: %d\n\r", recived);
						//set speed to package
						speed = recived;
						//increment
						recievedPacket++;
					
        }
				//set bool to false
				readIn = false;
				//clear interrupt
				//GPIOD->ICR =0x80;
			}
		//timer1 has interrupt for 5 secs prin tout packets dropped, sent, recieved
		if(AlertFiveSec)
		{
			AlertFiveSec = false;
			printf("Number of packets dropped: %d\nNumber of packets sent: %d\n",droppedPacket, sentPacket);
			printf("Number of packets recieved: %d\n", recievedPacket);
		}
	}
		//if correct answer
		if(correct)
		{		
			//increment score
			score++;			
			//send score to other board
			status = wireless_send_32(false, false, score);
			//Game wins at 50
			if(score == 50)
			{
				//End game with win screen
			 endGame(true, score);
			}
		}
		//if wrong...
		else
		{
			//send 0 to indicate that wrong
			status = wireless_send_32(false, false, 0);
			//print losing game image
			endGame(false, score);
		}
		
		//if the packet sent is a success increment
		if(status == NRF24L01_TX_SUCCESS)
		{
			sentPacket++;
		}
		//if dropped packet increment
		else if( status == NRF24L01_TX_PCK_LOST)
		{
			droppedPacket++;
		}
		
		//generate new button to be pressed and new lcd image
		buttonToBePressed = random_generate();
	}
	//Recieve Mode
	else
	{		
		//if external pin interrupt...
		if(readIn)
			{
				//read in wireless value
				status = wireless_get_32(false, &recived);
				//if the packet recieved was successful
        if(status == NRF24L01_RX_SUCCESS)
        {
						//print it out for debugging 
            //printf("Received: %d\n\r", recived);
						//increment recieved packets
						recievedPacket++;
						//if its not the fail packet then update score and print the score
						if(recived != 0 && recived != 50)
						{
							score = recived;
							printScore(score);
						}
						//If 50 win win screen!
						else if(recived == 50)
						{
							endGame(true, 50);
						}
						
						//if it is the fail packet
						else
						{
							//Lost print screen
							endGame(false,score);	
						}							
        }
				//set read in to false
				readIn = false;
				//clear interrupts
				//GPIOD->ICR =0x80;
			}
			else{
				//read pushbuttons in
				read_buttons(pressed);
				//if press up increment speed up to a max value, and thensend it to the master board
				if(*pressed == BTN_UP)
				{
						speed += 1;
					if(speed > 10000000)
						speed = 10000;
					if(sendSpeed)
					{
						//debug
						//printf("%d\n", speed);
						status = wireless_send_32(false, false, speed);
						sendSpeed = false;
					}
				}
				//if press down then decrement the speed to a min value and then send it t othe master board
				else if (*pressed == BTN_DOWN)
				{
						speed -= 1;
					if(speed < 1)
						speed = 10000;
					if(sendSpeed)
					{
						//printf("%d\n", speed);
						status = wireless_send_32(false, false, speed);
						sendSpeed = false;
					}
				}
				*pressed = 0;
			}
			
			//it packet sent is good increment sentPacket
		if(status == NRF24L01_TX_SUCCESS)
		{
			sentPacket++;
		}
		//if it was dropped increment droppedPacket
		else if( status == NRF24L01_TX_PCK_LOST)
		{
			droppedPacket++;
		}
			
		//If timer1 interrupt then print out packet stats to serial
		if(AlertFiveSec)
		{
			AlertFiveSec = false;
			
			printf("Number of packets dropped: %d\nNumber of packets sent: %d\n",droppedPacket, sentPacket);
			printf("Number of packets recieved: %d\n", recievedPacket);
		}
	}
	
	}
}
