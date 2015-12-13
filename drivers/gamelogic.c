#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
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

uint32_t data = 0xFFFF;
uint32_t ps2data = 0xFFFF;
uint32_t upcounter = 0;
uint32_t downcounter = 0;
uint32_t leftcounter = 0;
uint32_t rightcounter = 0;
uint32_t ps2counter = 0;
uint32_t x_data, y_data;
volatile bool initState = false;

void read_buttons(uint16_t *pressed)
{		
		initState = true;
	
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
				*pressed |= BTN_UP;
				upcounter = 0;				
			}
			initState = false;
		}
		else {
			upcounter = 0;
			initState &= true;
		}
		
		if( (data & DIR_BTN_DOWN) == 0)
		{
			downcounter++;
			if(downcounter >= 4)
			{
				*pressed |= BTN_DOWN;
				downcounter = 0;				
			}
			initState = false;
		}
		else {
			initState &= true;
			downcounter = 0;
		}
		
		if( (data & DIR_BTN_LEFT) == 0)
		{
			leftcounter++;
			if(leftcounter >= 4)
			{
				*pressed |= BTN_LEFT;
				leftcounter = 0;
				
			}
			initState = false;
		}
		else {
			initState &= true;
			leftcounter = 0;
		}
		
		if( (data & DIR_BTN_RIGHT) == 0)
		{
			rightcounter++;
			if(rightcounter >= 4)
			{
				*pressed |= BTN_RIGHT;
				rightcounter = 0;
				
			}
			initState = false;
		}
		else {
			initState &= true;
			rightcounter = 0;
		}
		
		if( ((ps2data & PS2_BTN) == 0))
		{
			ps2counter++;
			if(ps2counter >= 4)
			{
				edit_eeprom();
				ps2data = 0xffff;
				ps2counter = 0;
			}
			initState = false;
		}
		else {
			initState &= true;
			ps2counter = 0;
		}
		AlertSysTick = false;
	
		
		
		if(updateXY)
		{
			//while(!aiReady) {};
			//	aiReady = false;
				x_data = getADCValue(PS2_ADC_BASE,PS2_X_ADC_CHANNEL);
				y_data = getADCValue(PS2_ADC_BASE,PS2_Y_ADC_CHANNEL);
				//printf("X Dir value : 0x%03x        Y Dir value : 0x%03x\r",x_data, y_data);
				if(y_data > 0xa00)
				{
					initState = false;
					*pressed |= PS2_UP;
				}
				if(y_data < 0x500)
				{
					initState = false;
					*pressed |= PS2_DOWN;
				}
				if(x_data > 0xa00)
				{
					initState = false;
					*pressed |= PS2_LEFT;
				}
				if(x_data < 0x500)
				{
					initState = false;
					*pressed |= PS2_RIGHT;
				}
				if(!(x_data < 0x900 && x_data > 0x700 && y_data < 0x900 && y_data > 0x700))
					initState = false;
				else
					initState &= true;
					
				updateXY = false;
		}
		else
			initState = false;    

}

int random_generate(void)
{
	uint16_t x;
	x = rand();
	
	if( x <= 8191 )
	{
		print_lcd(push_up);
		return BTN_UP;
	}
	else if ( x <= 16382 && x > 8191 )
	{
		print_lcd(push_down);
		return BTN_DOWN;
	}
	else if ( x <= 24573 && x > 16382 )
	{
		print_lcd(push_left);
		return BTN_LEFT;
	}
	else if ( x <= 32764 && x > 24573 )
	{
		print_lcd(push_right);
		return BTN_RIGHT;
	}
	else if ( x <= 40955 && x > 32764 )
	{
		print_lcd(flick_up);
		return PS2_UP;
	}
	else if ( x <= 49146 && x > 40955 )
	{
		print_lcd(flick_down);
		return PS2_DOWN;
	}
	else if ( x <= 57337 && x > 49146 )
	{
		print_lcd(flick_left);
		return PS2_LEFT;
	}
	else
	{
		print_lcd(flick_right);
		return PS2_RIGHT;		
	}
}

void endGame(bool win, int score)
{
	char firstC;
	char secondC;
	
	firstC = (score % 10) + 0x30;
	secondC = (char) (((score / 10))+ 0x30);

	dogs102_clear();
	if(win)
	{
		dogs102_write_char_10pts(1, 'Y', 1);
		dogs102_write_char_10pts(1, 'o', 2);
		dogs102_write_char_10pts(1, 'u', 3);
		dogs102_write_char_10pts(1, ' ', 4);
		dogs102_write_char_10pts(1, 'W', 5);
		dogs102_write_char_10pts(1, 'i', 6);
		dogs102_write_char_10pts(1, 'n', 7);
		dogs102_write_char_10pts(1, '!', 8);
		dogs102_write_char_10pts(2, 'S', 1);
		dogs102_write_char_10pts(2, 'c', 2);
		dogs102_write_char_10pts(2, 'o', 3);
		dogs102_write_char_10pts(2, 'r', 4);
		dogs102_write_char_10pts(2, 'e', 5);
		dogs102_write_char_10pts(2, ':', 6);
		dogs102_write_char_10pts(2, secondC, 7);
		dogs102_write_char_10pts(2, firstC, 8);
	}
	else
	{
		dogs102_write_char_10pts(1, 'Y', 1);
		dogs102_write_char_10pts(1, 'o', 2);
		dogs102_write_char_10pts(1, 'u', 3);
		dogs102_write_char_10pts(1, ' ', 4);
		dogs102_write_char_10pts(1, 'L', 5);
		dogs102_write_char_10pts(1, 'o', 6);
		dogs102_write_char_10pts(1, 's', 7);
		dogs102_write_char_10pts(1, 't', 8);
		dogs102_write_char_10pts(2, 'S', 1);
		dogs102_write_char_10pts(2, 'c', 2);
		dogs102_write_char_10pts(2, 'o', 3);
		dogs102_write_char_10pts(2, 'r', 4);
		dogs102_write_char_10pts(2, 'e', 5);
		dogs102_write_char_10pts(2, ':', 6);
		dogs102_write_char_10pts(2, secondC, 7);
		dogs102_write_char_10pts(2, firstC, 8);
	}
	while(true);
}
