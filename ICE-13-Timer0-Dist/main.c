//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "timers.h"

// ADD CODE
// Create a constant called SEC_ONE that is equal to the number
// of clock cycles it will take for 1 second to pass.  
// 
// The clock frequency of the Tiva Launchpad has been configured for 
// 50HMz.
#define SEC_ONE   0

extern void serialDebugInit(void);


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  int i = 0;
  serialDebugInit();

  printf("\n\r");
  printf("************************************\n\r");
  printf("ECE353 - Timer0 In Class Exercise\n\r");
  printf("************************************\n\r");
  
  
  // ADD CODE
  // Add code to the infinite loop that increments i once every second.
  // When the count is updated, the new count should be printed out to
  // the serial debug interface.
  //
  // Format:
  //    SEC: 0
  //    SEC: 1
  //    SEC: 2
  while(1)
  {
  };
}
