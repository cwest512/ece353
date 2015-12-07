//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "gpio_port.h"
#include "adc.h"

extern void serialDebugInit(void);


// ADD CODE
// Define the base addresses for the GPIO port that is connected to the PS2 Joystick
#define   PS2_GPIO_BASE    0

// ADD CODE
// Define which pin number ( 0 through 7) each signal is connected to
#define   PS2_X_PIN_NUM      0
#define   PS2_Y_PIN_NUM      0

// Macros passed to the gpio functions that are used to configure the GPIO port.
#define   PS2_X_DIR_MASK     (1 << PS2_X_PIN_NUM)
#define   PS2_Y_DIR_MASK     (1 << PS2_Y_PIN_NUM)


// ADD CODE
// Define the base addresses of the the ADC you are going to use to either ADC0 or ADC1.
// Either will work.
#define   PS2_ADC_BASE     0


// ADD CODE
// Set the analog channel for each direction.  Each analog channel is associated with a
// single GPIO pin.  Make sure to see table 13-1 to see how the GPIO pin maps to an 
// analog channel.
#define   PS2_X_ADC_CHANNEL  0
#define   PS2_Y_ADC_CHANNEL  0

//*****************************************************************************
//*****************************************************************************
void print_ps2(void)
{
  uint32_t x_data, y_data;
  uint32_t i;
  while(1)
  {

    x_data = getADCValue(PS2_ADC_BASE,PS2_X_ADC_CHANNEL);
    y_data = getADCValue(PS2_ADC_BASE,PS2_Y_ADC_CHANNEL);
    printf("X Dir value : 0x%03x        Y Dir value : 0x%03x\r",x_data, y_data);
    for(i=0;i<1000000; i++){}
    
  }
}


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  serialDebugInit();

  printf("\n\r***************************************\n\r");
  printf("ECE353 - Analog Input In Class Exercise\n\r");
  printf("****************************************\n\r");
  
  // ADD CODE
  // Configure the 2 pins connected to the X and Y direction on the PS2 controller to
  // be analog inputs
	//
	//  *** USE the MACRO's you've defined above while configuring the GPIO PORT. ***
	//		PS2_GPIO_BASE
	//		PS2_X_DIR_MASK
	//		PS2_Y_DIR_MASK
	//
  //   i. Enable the gpio port used for the PS2
  //  ii. Set the PS2 pins as inputs
  // iii. Configure the 2 pins as analog pins
  // iv.  Configure the 2 pins as their alternate function


  // ADD code
  // Initialize the ADC used by the PS2 controller
	//
	// *** USE the MACRO's you've defined above while configuring the ADC ***
	//		PS2_ADC_BASE
	//		PS2_X_ADC_CHANNEL
	//		PS2_Y_ADC_CHANNEL
  
  // Get User Input
  print_ps2();
  
  // Reach infinite loop
  while(1){};
}
