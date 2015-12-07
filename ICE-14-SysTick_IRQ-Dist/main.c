//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "TM4C123.h"
#include "interrupts.h"

#define   DIR_BTN_UP      (1 << 1)
#define   DIR_BTN_DOWN    (1 << 4)
#define   DIR_BTN_LEFT    (1 << 3)
#define   DIR_BTN_RIGHT   (1 << 2)

extern void serialDebugInit(void);
extern void gpio_init(void);

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  serialDebugInit();
  gpio_init();
  
  printf("\n\r");
  printf("********************************************\n\r");
  printf("ECE353 - SysTick Interrupt In Class Exercise\n\r");
  printf("*********************************************\n\r");

  
  
  // Reach infinite loop
  while(1)
  {
    if(AlertSysTick)
    {
     // Read GPIOF and debounce the pushbuttons.  If a pushbutton has been held low four consecutive times in
		 // which AlertSysTick is true, then the button is considered to be pressed.
			
			AlertSysTick = false;
			
    }
  }
}
