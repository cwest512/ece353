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

//data for the GPIOF data
uint32_t data = 0xFFFF;

//ps2 pushbutton data for gpioe
uint32_t ps2data = 0xFFFF;

//counter for pushbutton up
uint32_t upcounter = 0;
//counter for pushbutton down
uint32_t downcounter = 0;
//counter for pushbutton left
uint32_t leftcounter = 0;
//counter for pushbutton right
uint32_t rightcounter = 0;
//counter for ps2 push
uint32_t ps2counter = 0;
//value for first highscore
uint8_t first;
//value for second highscore
uint8_t second;
//value for third high score
uint8_t third;

//reset varriable
bool reset = true;

//Bool for the intial state of the controller push buttons and ps2 stick
volatile bool initState = false;

//*****************************************************************************
// Read the push buttons, ps2 button, and ps2 x and y cords and respond with 
// appropriate responses
//	Para: Pressed - pointer to a varraible to see what was pressed.
//*****************************************************************************
void read_buttons(uint16_t *pressed)
{		
		//set the intial state to true for logic reasons
		initState = true;
	
		//If the Systick Timer has gone off and set to true then..
		if(AlertSysTick)
		{
			//Get data for pushbuttons and ps2 from respetive gpio
			data = GPIOF->DATA;
			ps2data = GPIOE->DATA;
		}
		
		//If the pushbutton is down then...
		if((data & DIR_BTN_UP) == 0)
		{
			//increment counter
			upcounter++;
			
			//if the total counter is greater than or equal to four
			if(upcounter >= 4)
			{
				//update the value of pressed to inclue BTN_UP
				*pressed |= BTN_UP;
				
				//reset upcounter
				upcounter = 0;				
			}
			//Say that its not in its initial state
			initState = false;
		}
		else {
			//reset upcounter
			upcounter = 0;
			//say that it is in its initial state, it it isn't already in a false state
			initState &= true;
		}
		
		//if the down pushbutton is pressed then ..
		if( (data & DIR_BTN_DOWN) == 0)
		{
			//increment counter
			downcounter++;
			//if counter is greater than or equal to four
			if(downcounter >= 4)
			{
				//update the vlaue of pressed to include BTN_DOWN
				*pressed |= BTN_DOWN;
				
				//reset the counter
				downcounter = 0;				
			}
			//Say that its not in an initial state
			initState = false;
		}
		else {
			//say that it is in its initial state, it it isn't already in a false state
			initState &= true;
			//reset counter
			downcounter = 0;
		}
		
		//If the left push button is pressed
		if( (data & DIR_BTN_LEFT) == 0)
		{
			//increment counter
			leftcounter++;
			//if counter is greater than or equal to four
			if(leftcounter >= 4)
			{
				//update pressed to include BTN_LEFT
				*pressed |= BTN_LEFT;
				
				//reset counter
				leftcounter = 0;
			}
			//say that its not in the intial state
			initState = false;
		}
		else {
			//say that it is in its initial state, it it isn't already in a false state
			initState &= true;
			//reset counter
			leftcounter = 0;
		}
		
		//if the right push button is pressed ...
		if( (data & DIR_BTN_RIGHT) == 0)
		{
			//increment the counter
			rightcounter++;
			//if the counter is greater than or equal to 4
			if(rightcounter >= 4)
			{
				//update pressed to include BTN_RIGHT
				*pressed |= BTN_RIGHT;
				//reset counter
				rightcounter = 0;
				
			}
			//Not in initial state
			initState = false;
		}
		else {
			//say that it is in its initial state, it it isn't already in a false state
			initState &= true;
			//reset counter
			rightcounter = 0;
		}
		
		//If the ps2 button is pressed then..
		if( ((ps2data & PS2_BTN) == 0))
		{
			//increment ocunter
			ps2counter++;
			//if the counter is greater than or equal to 4
			if(ps2counter >= 4)
			{
				//Edit EEPROM data
				edit_eeprom();
				//reset counter
				ps2counter = 0;
			}
			//Not in initial state
			initState = false;
		}
		else {
			//say that it is in its initial state, it it isn't already in a false state
			initState &= true;
			//reset counter
			ps2counter = 0;
		}
		//Set Systick bool to false
		AlertSysTick = false;

		//If timer0A bool is true then..
		if(updateXY)				
		{
				//if joystick data is up..
				if(ps2_y_data > 0xa00)
				{
					//Not in initial state
					initState = false;
					//update pressed to include PS2_UP
					*pressed |= PS2_UP;
				}
				//if joystick data is down..
				if(ps2_y_data < 0x500)
				{
					//Not in inital state
					initState = false;
					//update pressed to include PS2_DOWN
					*pressed |= PS2_DOWN;
				}
				//if joystick is left...
				if(ps2_x_data > 0xa00)
				{
					//Not in initial state
					initState = false;
					//update pressed to include PS2_LEFT
					*pressed |= PS2_LEFT;
				}
				//if joystick is right..
				if(ps2_x_data < 0x500)
				{
					//Not in initial state
					initState = false;
					//update pressed to include PS2_RIGHT
					*pressed |= PS2_RIGHT;
				}
				//if joystick is not within bounds of initail state (0x700-0x900 for both x and y)..
				if(!(ps2_x_data < 0x900 && ps2_x_data > 0x700 && ps2_y_data < 0x900 && ps2_y_data > 0x700))
					//Not in inital state
					initState = false;
				else
					//say that it is in its initial state, it it isn't already in a false state
					initState &= true;
				//Change timer0A bool to false
				updateXY = false;
			}
		else
			//Not in initial state if you cant check ps2 cords
			initState = false;
    

}

//*****************************************************************************
// Generate a random number to then use to print a varriable to the lcd and then
// start checking to see if it was pressed.
// return: value to check for being pressed
//*****************************************************************************
int random_generate(void)
{
	//Random uint16_t var
	uint16_t x;
	//Intialize the var
	x = rand();
	
	//Clear LCD
	dogs102_clear();
	
	//Each of the ifs catches everything a uint16_t rand() can produce and split up proportionately
	// for each given case, it then prints the specific image to lcd and returns a value to check for.
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

//*****************************************************************************
// Method for when the game is done, prints a "You Win!" or "You Lose" Screen,
// with respective scores
//
// Param: bool win - if they won or not
//				int score - the score they got
//*****************************************************************************
void endGame(bool win, int score)
{
	//Characters to split up the score
	char firstC;
	char secondC;
	
	//Get the least significant digit and turn it into a char
	firstC = (score % 10) + 0x30;
	
	//get the most significant digit and turn it into a char
	secondC = (char) (((score / 10))+ 0x30);

	//clear lcd
	dogs102_clear();
	
	//if they win then..
	if(win)
	{
		//Print out "You Win! Score:" Score
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
		//Print out "You Lose Score:" Score
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
	
	//Waits for a given amount of time to show screen, also checks for ps2 button press
	time_wait();
	
	//attempts to fill in high scores if possible
	high_scores(score);
	
	//prints the score
	printHighScores();
	
	//end of the game, wait until hardware reset
	while(1);	
		
}

//*****************************************************************************
// Prints the score to the "slave" board
//
// Param: int score - the score they currently have
//*****************************************************************************
void printScore(int score)
{
	//chars for to split up the score
	char firstC;
	char secondC;
	
	//Get the least significant digit and turn it into a char
	firstC = (score % 10) + 0x30;
	
	//get the most significant digit and turn it into a char
	secondC = (char) (((score / 10))+ 0x30);

	//Clear lcd and then print current score
		dogs102_clear();
		dogs102_write_char_10pts(0, 'C', 0);
		dogs102_write_char_10pts(0, 'u', 1);
		dogs102_write_char_10pts(0, 'r', 2);
		dogs102_write_char_10pts(0, 'r', 3);
		dogs102_write_char_10pts(0, 'e', 4);
		dogs102_write_char_10pts(0, 'n', 5);
		dogs102_write_char_10pts(0, 't', 6);
		dogs102_write_char_10pts(1, 'S', 1);
		dogs102_write_char_10pts(1, 'c', 2);
		dogs102_write_char_10pts(1, 'o', 3);
		dogs102_write_char_10pts(1, 'r', 4);
		dogs102_write_char_10pts(1, 'e', 5);
		dogs102_write_char_10pts(1, ':', 6);
		dogs102_write_char_10pts(1, secondC, 8);
		dogs102_write_char_10pts(1, firstC, 9);
}

//*****************************************************************************
// Adds new score to high score eeprom list if appliciable
//
// Param: int score - the score they got
//*****************************************************************************
void high_scores(int score)
{
	//Get first second and third high scores from eeprom
	eeprom_byte_read(EEPROM_I2C_BASE, 162, &first );
	eeprom_byte_read(EEPROM_I2C_BASE, 163, &second );
	eeprom_byte_read(EEPROM_I2C_BASE, 164, &third );
	
	//If new score is greater than first..
	if(first < score)
	{
		third = second;
		second = first;
		first = score;
	}
	//If new score is greater than second
	else if (second < score)
	{
		third = second;
		second = score;
	}
	//if new score is greater than third
	else if ( third < score)
	{
		third = score;
	}
	//Write out new scores to eeprom
	eeprom_byte_write(EEPROM_I2C_BASE, 162, first );
	eeprom_byte_write(EEPROM_I2C_BASE, 163, second);
	eeprom_byte_write(EEPROM_I2C_BASE, 164, third);
}

//*****************************************************************************
// Prints High Scores
//*****************************************************************************
void printHighScores(void)
{
	//Chars to split up scores
	char firstC;
	char secondC;
		
	//Get the least significant digit and turn it into a char
	firstC = (first % 10) + 0x30;
	
	//get the most significant digit and turn it into a char
	secondC = (char) (((first / 10))+ 0x30);
	
	//LCD clear
	dogs102_clear();
	//print out "Highscores" and the first high score
		dogs102_write_char_10pts(0, 'H', 0);
		dogs102_write_char_10pts(0, 'i', 1);
		dogs102_write_char_10pts(0, 'g', 2);
		dogs102_write_char_10pts(0, 'h', 3);
		dogs102_write_char_10pts(0, 'S', 4);
		dogs102_write_char_10pts(0, 'c', 5);
		dogs102_write_char_10pts(0, 'o', 6);
		dogs102_write_char_10pts(0, 'r', 7);
		dogs102_write_char_10pts(0, 'e', 8);
		dogs102_write_char_10pts(0, 's', 9);
		dogs102_write_char_10pts(1, '1', 2);
		dogs102_write_char_10pts(1, 's', 3);
		dogs102_write_char_10pts(1, 't', 4);
		dogs102_write_char_10pts(1, ':', 5);
		dogs102_write_char_10pts(1, secondC, 8);
		dogs102_write_char_10pts(1, firstC, 9);
		
		//Get the least significant digit and turn it into a char
		firstC = (second % 10) + 0x30;
		
		//get the most significant digit and turn it into a char
		secondC = (char) (((second / 10))+ 0x30);
	
		//print out 2nd high score
		dogs102_write_char_10pts(2, '2', 2);
		dogs102_write_char_10pts(2, 'n', 3);
		dogs102_write_char_10pts(2, 'd', 4);
		dogs102_write_char_10pts(2, ':', 5);
		dogs102_write_char_10pts(2, secondC, 8);
		dogs102_write_char_10pts(2, firstC, 9);
		
		//Get the least significant digit and turn it into a char
		firstC = (third % 10) + 0x30;
		//get the most significant digit and turn it into a char
		secondC = (char) (((third / 10))+ 0x30);
	
		//print out third high score
		dogs102_write_char_10pts(3, '3', 2);
		dogs102_write_char_10pts(3, 'r', 3);
		dogs102_write_char_10pts(3, 'd', 4);
		dogs102_write_char_10pts(3, ':', 5);
		dogs102_write_char_10pts(3, secondC, 8);
		dogs102_write_char_10pts(3, firstC, 9);

}

//*****************************************************************************
// used to select transmit(master) mode or recieve(slave) mode
// returns: bool - mode 
//*****************************************************************************
bool mode_selector(void)
{
	//which mode
	bool mode;
	
	//Busy wait bool for choice on mode
	bool waitForChoice = true;
	//clear lcd
	dogs102_clear();
	
	//Welcome screen
		dogs102_write_char_10pts(0, ' ', 0);
		dogs102_write_char_10pts(0, 'W', 1);
		dogs102_write_char_10pts(0, 'e', 2);
		dogs102_write_char_10pts(0, 'l', 3);
		dogs102_write_char_10pts(0, 'c', 4);
		dogs102_write_char_10pts(0, 'o', 5);
		dogs102_write_char_10pts(0, 'm', 6);
		dogs102_write_char_10pts(0, 'e', 7);
		dogs102_write_char_10pts(0, '!', 8);
		dogs102_write_char_10pts(0, ' ', 9);
	
	//Wait
		time_wait();
		
	//Clear LCD
	dogs102_clear();
	//Choose mode screen
		dogs102_write_char_10pts(0, 'C', 0);
		dogs102_write_char_10pts(0, 'h', 1);
		dogs102_write_char_10pts(0, 'o', 2);
		dogs102_write_char_10pts(0, 'o', 3);
		dogs102_write_char_10pts(0, 's', 4);
		dogs102_write_char_10pts(0, 'e', 5);
		dogs102_write_char_10pts(0, 'M', 6);
		dogs102_write_char_10pts(0, 'o', 7);
		dogs102_write_char_10pts(0, 'd', 8);
		dogs102_write_char_10pts(0, 'e', 9);
		
		dogs102_write_char_10pts(1, 'U', 0);
		dogs102_write_char_10pts(1, 'p', 1);
		dogs102_write_char_10pts(1, ' ', 2);
		dogs102_write_char_10pts(1, 'f', 3);
		dogs102_write_char_10pts(1, 'o', 4);
		dogs102_write_char_10pts(1, 'r', 5);
		dogs102_write_char_10pts(1, ' ', 6);
		dogs102_write_char_10pts(1, 'R', 7);
		dogs102_write_char_10pts(1, 'e', 8);
		dogs102_write_char_10pts(1, 'c', 9);
		
		dogs102_write_char_10pts(2, 'D', 0);
		dogs102_write_char_10pts(2, 'o', 1);
		dogs102_write_char_10pts(2, 'w', 2);
		dogs102_write_char_10pts(2, 'n', 3);
		dogs102_write_char_10pts(2, ' ', 4);
		dogs102_write_char_10pts(2, 't', 5);
		dogs102_write_char_10pts(2, 'o', 6);
		dogs102_write_char_10pts(2, ' ', 7);
		dogs102_write_char_10pts(2, ' ', 8);
		dogs102_write_char_10pts(2, ' ', 9);
		
		dogs102_write_char_10pts(3, 'P', 0);
		dogs102_write_char_10pts(3, 'l', 1);
		dogs102_write_char_10pts(3, 'a', 2);
		dogs102_write_char_10pts(3, 'y', 3);
		dogs102_write_char_10pts(3, ' ', 4);
		dogs102_write_char_10pts(3, 'g', 5);
		dogs102_write_char_10pts(3, 'a', 6);
		dogs102_write_char_10pts(3, 'm', 7);
		dogs102_write_char_10pts(3, 'e', 8);
		dogs102_write_char_10pts(3, '!', 9);
		
		//Busy wait for choice on mode or eeprom edit
		while(waitForChoice)
		{
			//Wait for systick bool
			if(AlertSysTick)
			{
				//update data
				data = GPIOF->DATA;
				ps2data = GPIOE->DATA;
			}
			
			//if up button pressed
		if((data & DIR_BTN_UP) == 0)
		{
			//incrmeent counter
			upcounter++;
			//if counter greater than or equal to 4
			if(upcounter >= 4)
			{
				//change busy wait
				waitForChoice = false;
				//recieve mode
				mode = false;
				//clear counter
				upcounter = 0;				
			}
		}
		else {
			//clear counter
			upcounter = 0;
		}
		
		//if down button pressed
		if( (data & DIR_BTN_DOWN) == 0)
		{
			//increment counter
			downcounter++;
			//if ocunter is greater than or equal 4
			if(downcounter >= 4)
			{
				//change busy wait varriable
				waitForChoice = false;
				//Transmit mode
				mode = true;
				//clear counter
				downcounter = 0;				
			}
		}
		else {
			//clear counter
			downcounter = 0;
			}
		
		//if ps2 pressed
		if( ((ps2data & PS2_BTN) == 0))
		{
			//increment ocunter
			ps2counter++;
			//if counter is greater than or eqaul to 4
			if(ps2counter >= 4)
			{
				//edit eeprom
				edit_eeprom();
				//clear counter
				ps2counter = 0;
			}
		}
		else {
			//clear counter
			ps2counter = 0;
			}
		}
		
		//clear lcd
		dogs102_clear();
		//if transmit mode then print transmit screen
		if(mode)
		{
		dogs102_write_char_10pts(0, ' ', 0);
		dogs102_write_char_10pts(0, 'T', 1);
		dogs102_write_char_10pts(0, 'r', 2);
		dogs102_write_char_10pts(0, 'a', 3);
		dogs102_write_char_10pts(0, 'n', 4);
		dogs102_write_char_10pts(0, 's', 5);
		dogs102_write_char_10pts(0, 'm', 6);
		dogs102_write_char_10pts(0, 'i', 7);
		dogs102_write_char_10pts(0, 't', 8);
		dogs102_write_char_10pts(0, ' ', 9);
			
		dogs102_write_char_10pts(1, ' ', 0);
		dogs102_write_char_10pts(1, ' ', 1);
		dogs102_write_char_10pts(1, ' ', 2);
		dogs102_write_char_10pts(1, 'M', 3);
		dogs102_write_char_10pts(1, 'o', 4);
		dogs102_write_char_10pts(1, 'd', 5);
		dogs102_write_char_10pts(1, 'e', 6);
		dogs102_write_char_10pts(1, ' ', 7);
		dogs102_write_char_10pts(1, ' ', 8);
		dogs102_write_char_10pts(1, ' ', 9);
		
		dogs102_write_char_10pts(2, ' ', 0);
		dogs102_write_char_10pts(2, 'S', 1);
		dogs102_write_char_10pts(2, 'e', 2);
		dogs102_write_char_10pts(2, 'l', 3);
		dogs102_write_char_10pts(2, 'e', 4);
		dogs102_write_char_10pts(2, 'c', 5);
		dogs102_write_char_10pts(2, 't', 6);
		dogs102_write_char_10pts(2, 'e', 7);
		dogs102_write_char_10pts(2, 'd', 8);
		dogs102_write_char_10pts(2, ' ', 9);
		time_wait();
		return mode;
		}
		//if recieve mode print recieve screen
		else
		{
		dogs102_write_char_10pts(0, ' ', 0);
		dogs102_write_char_10pts(0, 'R', 1);
		dogs102_write_char_10pts(0, 'e', 2);
		dogs102_write_char_10pts(0, 'c', 3);
		dogs102_write_char_10pts(0, 'e', 4);
		dogs102_write_char_10pts(0, 'i', 5);
		dogs102_write_char_10pts(0, 'v', 6);
		dogs102_write_char_10pts(0, 'e', 7);
		dogs102_write_char_10pts(0, ' ', 8);
		dogs102_write_char_10pts(0, ' ', 9);
			
		dogs102_write_char_10pts(1, ' ', 0);
		dogs102_write_char_10pts(1, ' ', 1);
		dogs102_write_char_10pts(1, ' ', 2);
		dogs102_write_char_10pts(1, 'M', 3);
		dogs102_write_char_10pts(1, 'o', 4);
		dogs102_write_char_10pts(1, 'd', 5);
		dogs102_write_char_10pts(1, 'e', 6);
		dogs102_write_char_10pts(1, ' ', 7);
		dogs102_write_char_10pts(1, ' ', 8);
		dogs102_write_char_10pts(1, ' ', 9);
		
		dogs102_write_char_10pts(2, ' ', 0);
		dogs102_write_char_10pts(2, 'S', 1);
		dogs102_write_char_10pts(2, 'e', 2);
		dogs102_write_char_10pts(2, 'l', 3);
		dogs102_write_char_10pts(2, 'e', 4);
		dogs102_write_char_10pts(2, 'c', 5);
		dogs102_write_char_10pts(2, 't', 6);
		dogs102_write_char_10pts(2, 'e', 7);
		dogs102_write_char_10pts(2, 'd', 8);
		dogs102_write_char_10pts(2, ' ', 9);
		time_wait();
		return mode;
		}
		
	}

	
//*****************************************************************************
// Instruction screens for recive mode
//*****************************************************************************	
void instructionsRecv(void)
{
	
		dogs102_clear();
		dogs102_write_char_10pts(0, 'H', 0);
		dogs102_write_char_10pts(0, 'o', 1);
		dogs102_write_char_10pts(0, 'w', 2);
		dogs102_write_char_10pts(0, ' ', 3);
		dogs102_write_char_10pts(0, 't', 4);
		dogs102_write_char_10pts(0, 'o', 5);
		dogs102_write_char_10pts(0, ' ', 6);
		dogs102_write_char_10pts(0, 'R', 7);
		dogs102_write_char_10pts(0, 'e', 8);
		dogs102_write_char_10pts(0, 'c', 9);
			
		dogs102_write_char_10pts(1, 'U', 0);
		dogs102_write_char_10pts(1, 'p', 1);
		dogs102_write_char_10pts(1, ' ', 2);
		dogs102_write_char_10pts(1, 't', 3);
		dogs102_write_char_10pts(1, 'o', 4);
		dogs102_write_char_10pts(1, ' ', 5);
		dogs102_write_char_10pts(1, 'i', 6);
		dogs102_write_char_10pts(1, 'n', 7);
		dogs102_write_char_10pts(1, 'c', 8);
		dogs102_write_char_10pts(1, ' ', 9);
		
		dogs102_write_char_10pts(2, 'S', 0);
		dogs102_write_char_10pts(2, 'p', 1);
		dogs102_write_char_10pts(2, 'e', 2);
		dogs102_write_char_10pts(2, 'e', 3);
		dogs102_write_char_10pts(2, 'd', 4);
		dogs102_write_char_10pts(2, ' ', 5);
		dogs102_write_char_10pts(2, 'o', 6);
		dogs102_write_char_10pts(2, 'f', 7);
		dogs102_write_char_10pts(2, ' ', 8);
		dogs102_write_char_10pts(2, ' ', 9);
		
		dogs102_write_char_10pts(3, 'G', 0);
		dogs102_write_char_10pts(3, 'a', 1);
		dogs102_write_char_10pts(3, 'm', 2);
		dogs102_write_char_10pts(3, 'e', 3);
		dogs102_write_char_10pts(3, ' ', 4);
		dogs102_write_char_10pts(3, 'b', 5);
		dogs102_write_char_10pts(3, 'o', 6);
		dogs102_write_char_10pts(3, 'a', 7);
		dogs102_write_char_10pts(3, 'r', 8);
		dogs102_write_char_10pts(3, 'd', 9);
		
		time_wait();
		dogs102_clear();
		
		dogs102_write_char_10pts(0, 'H', 0);
		dogs102_write_char_10pts(0, 'o', 1);
		dogs102_write_char_10pts(0, 'w', 2);
		dogs102_write_char_10pts(0, ' ', 3);
		dogs102_write_char_10pts(0, 't', 4);
		dogs102_write_char_10pts(0, 'o', 5);
		dogs102_write_char_10pts(0, ' ', 6);
		dogs102_write_char_10pts(0, 'R', 7);
		dogs102_write_char_10pts(0, 'e', 8);
		dogs102_write_char_10pts(0, 'c', 9);
			
		dogs102_write_char_10pts(1, 'D', 0);
		dogs102_write_char_10pts(1, 'o', 1);
		dogs102_write_char_10pts(1, 'w', 2);
		dogs102_write_char_10pts(1, 'n', 3);
		dogs102_write_char_10pts(1, 'T', 4);
		dogs102_write_char_10pts(1, 'o', 5);
		dogs102_write_char_10pts(1, 's', 6);
		dogs102_write_char_10pts(1, 'l', 7);
		dogs102_write_char_10pts(1, 'o', 8);
		dogs102_write_char_10pts(1, 'w', 9);
		
		dogs102_write_char_10pts(2, 'S', 0);
		dogs102_write_char_10pts(2, 'p', 1);
		dogs102_write_char_10pts(2, 'e', 2);
		dogs102_write_char_10pts(2, 'e', 3);
		dogs102_write_char_10pts(2, 'd', 4);
		dogs102_write_char_10pts(2, ' ', 5);
		dogs102_write_char_10pts(2, 'o', 6);
		dogs102_write_char_10pts(2, 'f', 7);
		dogs102_write_char_10pts(2, ' ', 8);
		dogs102_write_char_10pts(2, ' ', 9);
		
		dogs102_write_char_10pts(3, 'G', 0);
		dogs102_write_char_10pts(3, 'a', 1);
		dogs102_write_char_10pts(3, 'm', 2);
		dogs102_write_char_10pts(3, 'e', 3);
		dogs102_write_char_10pts(3, ' ', 4);
		dogs102_write_char_10pts(3, 'b', 5);
		dogs102_write_char_10pts(3, 'o', 6);
		dogs102_write_char_10pts(3, 'a', 7);
		dogs102_write_char_10pts(3, 'r', 8);
		dogs102_write_char_10pts(3, 'd', 9);
		
		time_wait();
}

//*****************************************************************************
// Instructions screens for transmit mode 
//*****************************************************************************
void instructionsGame(void)
{
		dogs102_clear();
		dogs102_write_char_10pts(0, 'H', 0);
		dogs102_write_char_10pts(0, 'o', 1);
		dogs102_write_char_10pts(0, 'w', 2);
		dogs102_write_char_10pts(0, ' ', 3);
		dogs102_write_char_10pts(0, 't', 4);
		dogs102_write_char_10pts(0, 'o', 5);
		dogs102_write_char_10pts(0, ' ', 6);
		dogs102_write_char_10pts(0, 'R', 7);
		dogs102_write_char_10pts(0, 'e', 8);
		dogs102_write_char_10pts(0, 'c', 9);
			
		dogs102_write_char_10pts(1, 'C', 0);
		dogs102_write_char_10pts(1, 'l', 1);
		dogs102_write_char_10pts(1, 'i', 2);
		dogs102_write_char_10pts(1, 'c', 3);
		dogs102_write_char_10pts(1, 'k', 4);
		dogs102_write_char_10pts(1, ' ', 5);
		dogs102_write_char_10pts(1, 'B', 6);
		dogs102_write_char_10pts(1, 't', 7);
		dogs102_write_char_10pts(1, 'n', 8);
		dogs102_write_char_10pts(1, ' ', 9);
		
		dogs102_write_char_10pts(2, 'o', 0);
		dogs102_write_char_10pts(2, 'r', 1);
		dogs102_write_char_10pts(2, ' ', 2);
		dogs102_write_char_10pts(2, 'f', 3);
		dogs102_write_char_10pts(2, 'l', 4);
		dogs102_write_char_10pts(2, 'i', 5);
		dogs102_write_char_10pts(2, 'c', 6);
		dogs102_write_char_10pts(2, 'k', 7);
		dogs102_write_char_10pts(2, ' ', 8);
		dogs102_write_char_10pts(2, ' ', 9);
		
		dogs102_write_char_10pts(3, 't', 0);
		dogs102_write_char_10pts(3, 'h', 1);
		dogs102_write_char_10pts(3, 'e', 2);
		dogs102_write_char_10pts(3, 'c', 3);
		dogs102_write_char_10pts(3, 'o', 4);
		dogs102_write_char_10pts(3, 'r', 5);
		dogs102_write_char_10pts(3, 'r', 6);
		dogs102_write_char_10pts(3, 'e', 7);
		dogs102_write_char_10pts(3, 'c', 8);
		dogs102_write_char_10pts(3, 't', 9);
		
		time_wait();
		dogs102_clear();
		
		dogs102_write_char_10pts(0, 'H', 0);
		dogs102_write_char_10pts(0, 'o', 1);
		dogs102_write_char_10pts(0, 'w', 2);
		dogs102_write_char_10pts(0, ' ', 3);
		dogs102_write_char_10pts(0, 't', 4);
		dogs102_write_char_10pts(0, 'o', 5);
		dogs102_write_char_10pts(0, ' ', 6);
		dogs102_write_char_10pts(0, 'R', 7);
		dogs102_write_char_10pts(0, 'e', 8);
		dogs102_write_char_10pts(0, 'c', 9);
			
		dogs102_write_char_10pts(1, 'D', 0);
		dogs102_write_char_10pts(1, 'o', 1);
		dogs102_write_char_10pts(1, 'n', 2);
		dogs102_write_char_10pts(1, 't', 3);
		dogs102_write_char_10pts(1, ' ', 4);
		dogs102_write_char_10pts(1, 'G', 5);
		dogs102_write_char_10pts(1, 'e', 6);
		dogs102_write_char_10pts(1, 't', 7);
		dogs102_write_char_10pts(1, ' ', 8);
		dogs102_write_char_10pts(1, ' ', 9);
		
		dogs102_write_char_10pts(2, 'i', 0);
		dogs102_write_char_10pts(2, 't', 1);
		dogs102_write_char_10pts(2, ' ', 2);
		dogs102_write_char_10pts(2, 'w', 3);
		dogs102_write_char_10pts(2, 'r', 4);
		dogs102_write_char_10pts(2, 'o', 5);
		dogs102_write_char_10pts(2, 'n', 6);
		dogs102_write_char_10pts(2, 'g', 7);
		dogs102_write_char_10pts(2, '!', 8);
		dogs102_write_char_10pts(2, ' ', 9);

		time_wait();
}

//*****************************************************************************
// Wait a specific amount of time and check for ps2 presses
//*****************************************************************************
void time_wait(void)
{
	//increment varraible
	int i;
	//long loop to hold time
	for(i = 0; i < 1000000; i++) 
	{
		//Check if ps2 is pressed, then edit eeprom if it is
		if(AlertSysTick)
			ps2data = GPIOE->DATA;
		
		if( ((ps2data & PS2_BTN) == 0))
		{
			ps2counter++;
			if(ps2counter >= 4)
			{
				edit_eeprom();
				ps2counter = 0;
			}
		}
		else {
			ps2counter = 0;
		}
	}
}
