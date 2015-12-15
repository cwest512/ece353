//*****************************************************************************
// gamelogic.c
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
#include "lcd.h"

#define BTN_UP 0x1
#define BTN_DOWN 0x2
#define BTN_LEFT 0x4
#define BTN_RIGHT 0x8
#define BTN_PS2 0x10
#define PS2_UP 0x20
#define PS2_DOWN 0x40
#define PS2_LEFT 0x80
#define PS2_RIGHT 0x100

extern volatile bool initState;

//*****************************************************************************
// Read the push buttons, ps2 button, and ps2 x and y cords and respond with 
// appropriate responses
//	Para: Pressed - pointer to a varraible to see what was pressed.
//*****************************************************************************
void read_buttons(uint16_t *pressed);
//*****************************************************************************
// Generate a random number to then use to print a varriable to the lcd and then
// start checking to see if it was pressed.
// return: value to check for being pressed
//*****************************************************************************
int random_generate(void);
//*****************************************************************************
// Method for when the game is done, prints a "You Win!" or "You Lose" Screen,
// with respective scores
//
// Param: bool win - if they won or not
//				int score - the score they got
//*****************************************************************************
void endGame(bool win, int score);
//*****************************************************************************
// Adds new score to high score eeprom list if appliciable
//
// Param: int score - the score they got
//*****************************************************************************
void high_scores(int score);
//*****************************************************************************
// Prints High Scores
//*****************************************************************************
void printHighScores(void);
//*****************************************************************************
// used to select transmit(master) mode or recieve(slave) mode
// returns: bool - mode 
//*****************************************************************************
bool mode_selector(void);
//*****************************************************************************
// Instruction screens for recive mode
//*****************************************************************************	
void instructionsRecv(void);
//*****************************************************************************
// Instructions screens for transmit mode 
//*****************************************************************************
void instructionsGame(void);
//*****************************************************************************
// Wait a specific amount of time and check for ps2 presses
//*****************************************************************************
void time_wait(void);
//*****************************************************************************
// Prints the score to the "slave" board
//
// Param: int score - the score they currently have
//*****************************************************************************
void printScore(int score);
