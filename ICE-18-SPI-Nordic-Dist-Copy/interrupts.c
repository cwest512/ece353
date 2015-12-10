#include "interrupts.h"

volatile bool AlertOneSec = false;

 /****************************************************************************
 * The SysTick Handler 
 *
 * Register Information on the SysTick timer can be found on pages 129-132 
 * of the MCU data sheet.
 ****************************************************************************/
void SysTick_Handler(void)
{
  static int count = 0;
  
  // Clear the SysTick Counter 
  uint32_t val = SysTick->VAL;// Clears the SysTick Interrupt

	if ( count == 5)
	{
			AlertOneSec = true;
			count = 0;
	}
	else
	{
		count++;
	}
  
}


