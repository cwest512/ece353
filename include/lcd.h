#ifndef __LCD_H__
#define __LCD_H__
// Copyright (c) 2014, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <stdint.h>
#include "spi.h"
#include "gpio_port.h"
#include "fonts.h"

#define NUM_PAGES   8
#define NUM_COLS    102


//*****************************************************************************
// Fill out the #defines below to configure which pins are connected to
// the LCD Screen
// 
// ADD CODE
//*****************************************************************************
#define   LCD_GPIO_BASE       GPIOA_BASE
#define   LCD_SPI_BASE        SSI0_BASE
#define   LCD_CLK_PIN         PA2
#define   LCD_CS_PIN          PA3
#define   LCD_MOSI_PIN        PA5

#define   LCD_CLK_PIN_PCTL    GPIO_PCTL_PA2_SSI0CLK
#define   LCD_CS_PIN_PCTL     GPIO_PCTL_PA3_SSI0FSS
#define   LCD_MOSI_PIN_PCTL   GPIO_PCTL_PA5_SSI0TX


#define   GPIO_LCD_CD_BASE    GPIOA_BASE
#define   LCD_CD_PIN          PA4
#define 	LCD_CD_PORT					GPIOA

#define   GPIO_LCD_RST_N_BASE GPIOC_BASE
#define   LCD_RST_N_PIN       PC7
#define   LCD_RST_N_PORT			GPIOC

extern uint8_t push_up[];
extern uint8_t push_down[];
extern uint8_t push_left[];
extern uint8_t push_right[];
extern uint8_t push_ps2[];
extern uint8_t flick_up[];
extern uint8_t flick_down[];
extern uint8_t flick_left[];
extern  uint8_t flick_right[];

//*****************************************************************************
// Provided initialization code for the LCD.  Found in the data sheet.
//*****************************************************************************
void dogs102_init(void);

//*****************************************************************************
// Set the page address to page 7-0.  The orientation of the 353 carrier card
// puts page 0 at the top of the display and page 7 at the bottom.
//
// See "Set Page Address" in the EADOGS102W-6 data sheet, pg 5.
//
// Make sure to set the command mode correctly!
//*****************************************************************************
void dogs102_set_page(uint8_t   page);

//*****************************************************************************
// There are 102 columns in the display.  Use this function to set which colum
// data will be written to.
//
// See "Set Column Address LSB and MSB" in the EADOGS102W-6 data sheet, pg 5.
// This will require two different SPI transactions.
//
// Make sure to set the command mode correctly!
//*****************************************************************************
void dogs102_set_column(uint8_t   column);

//*****************************************************************************
// When not in command mode, any data written to the LCD is used to determine
// which pixels are turned ON/OFF for the curretnly active page.  A 1 turns a 
// pixel on and a 0 turns the pixel off.
//*****************************************************************************
void dogs102_write_data( uint8_t   data);
  

//*****************************************************************************
// Supports writing a 10 point character to the LCD.  There are "4" lines on 
// the LCD,
//*****************************************************************************
void dogs102_write_char_10pts( uint8_t line, char c, uint8_t charIndex);

//*****************************************************************************
// Used to clear the LCD of all pixels
//*****************************************************************************
void dogs102_clear(void);


//*****************************************************************************
// Initialize the LCD peripheral
//*****************************************************************************
void lcd_init(void);

#endif
