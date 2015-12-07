//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "gpio_port.h"

extern void serialDebugInit(void);


#define   DIR_BTN_BASE    GPIOF_BASE
#define   PS2_BTN_BASE    GPIOE_BASE

// ADD CODE
// Define the Bitmask for each button below.  You can create the bit mask
// by shifting 1 to the left by the pin number
#define   DIR_BTN_UP      (1 << 0)
#define   DIR_BTN_DOWN    (1 << 0)
#define   DIR_BTN_LEFT    (1 << 0)
#define   DIR_BTN_RIGHT   (1 << 0)

#define   PS2_BTN         (1 << 0)


//*****************************************************************************
//*****************************************************************************
void print_buttons(void)
{
  uint8_t data;
  uint32_t i;
  while(1)
  {
    data = GPIOF->DATA;
    
    if( (data & DIR_BTN_UP) == 0)
    {
      printf("BTN_UP pressed\n\r");
    }
    
    if( (data & DIR_BTN_DOWN) == 0)
    {
      printf("BTN_DOWN pressed\n\r");
    }
    
    if( (data & DIR_BTN_RIGHT) == 0)
    {
      printf("BTN_RIGHT pressed\n\r");
    }
    
    if( (data & DIR_BTN_LEFT) == 0)
    {
      printf("BTN_LEFT pressed\n\r");
    }
    
    if((GPIOE->DATA & PS2_BTN) == 0)
    {
      printf("PS2_BTN pressed\n\r");
    }
    
    for(i=0;i<100000; i++){}
    
  }
}


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  serialDebugInit();

  printf("\n\r************************************\n\r");
  printf("ECE353 - GPIO In Class Exercise\n\r");
  printf("************************************\n\r");
  
  // ADD CODE
  // Configure the 5 buttuns connected to the pushbuttons on the ECE353 Carrier
  //  i.	Enable the port used for the pushbuttons
  // ii.	Configure the 5 pushbuttons as digital pins
  // iii.	Configure the 5 pushbuttons as inputs
  // iv.	Configure the 5 pushbuttons with internal pull-up resistors

  // Use the #defines for DIR_BTN_BASE, DIR_BTN_UP, DIR_BTN_DOWN, 
  // DIR_BTN_LEFT, DIR_BTN_RIGHT when calling the configuration routines.
  
  // Use the #defines for PS2_BTN_BASE and PS2_BTN when calling the configuration routines.

  
  
  // Get User Input
  print_buttons();
  
  // Reach infinite loop
  while(1){};
}
