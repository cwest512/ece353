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
volatile bool initState = true;

void read_buttons(uint16_t *pressed)
{		
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
				initState = false;
			}
		}
		else {
			upcounter = 0;
			initState = true;
		}
		
		if( (data & DIR_BTN_DOWN) == 0)
		{
			downcounter++;
			if(downcounter >= 4)
			{
				*pressed |= BTN_DOWN;
				downcounter = 0;
				initState = false;
			}
		}
		else {
			initState = true;
			downcounter = 0;
		}
		
		if( (data & DIR_BTN_LEFT) == 0)
		{
			leftcounter++;
			if(leftcounter >= 4)
			{
				*pressed |= BTN_LEFT;
				leftcounter = 0;
				initState = false;
			}
		}
		else {
			initState = true;
			leftcounter = 0;
		}
		
		if( (data & DIR_BTN_RIGHT) == 0)
		{
			rightcounter++;
			if(rightcounter >= 4)
			{
				*pressed |= BTN_RIGHT;
				rightcounter = 0;
				initState = false;
			}
		}
		else {
			initState = true;
			rightcounter = 0;
		}
		
		if( ((ps2data & PS2_BTN) == 0))
		{
			ps2counter++;
			if(ps2counter >= 4)
			{
				*pressed |= BTN_PS2;
				ps2counter = 0;
				initState = false;
			}
		}
		else {
			initState = true;
			ps2counter = 0;
		}
		AlertSysTick = false;
		if(updateXY)
		{
				x_data = getADCValue(PS2_ADC_BASE,PS2_X_ADC_CHANNEL);
				y_data = getADCValue(PS2_ADC_BASE,PS2_Y_ADC_CHANNEL);
				printf("X Dir value : 0x%03x        Y Dir value : 0x%03x\r",x_data, y_data);
				if(y_data > 0xc00)
				{
					initState = false;
					*pressed |= PS2_UP;
				}
				if(y_data < 0x400)
				{
					initState = false;
					*pressed |= PS2_DOWN;
				}
				if(x_data > 0xc00)
				{
					initState = false;
					*pressed |= PS2_LEFT;
				}
				if(x_data < 0x400)
				{
					initState = false;
					*pressed |= PS2_RIGHT;
				}
				if(x_data < 0xa00 && x_data > 0x900 && y_data < 0xa00 && y_data > 0x900)
					initState = true;
				updateXY = false;
		}
//			  memset(input,0,80);
//				printf("\n\rEnter a string: ");
//				scanf("%79[^\n]", input);
//				printf("You entered: %s\n\r",input);
    

}

int random_generate(void)
{
	uint16_t x;
	x = rand()/2;
	
	if( x <= 3640 )
	{
		print_lcd(push_up);
		return BTN_UP;
	}
	else if ( x <= 7280 && x > 3640 )
	{
		print_lcd(push_down);
		return BTN_DOWN;
	}
	else if ( x <= 10920 && x > 7280 )
	{
		print_lcd(push_left);
		return BTN_LEFT;
	}
	else if ( x <= 14560 && x > 10920 )
	{
		print_lcd(push_right);
		return BTN_RIGHT;
	}
	else if ( x <= 18200 && x > 14560 )
	{
		print_lcd(push_ps2);
		return BTN_PS2;
	}
	else if ( x <= 21840 && x > 18200 )
	{
		print_lcd(flick_up);
		return PS2_UP;
	}
	else if ( x <= 25480 && x > 21840 )
	{
		print_lcd(flick_down);
		return PS2_DOWN;
	}
	else if ( x <= 29120 && x > 25480 )
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


