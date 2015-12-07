//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "board_config.h"


/******************************************************************************
 * Global Variables
 *****************************************************************************/



//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  char input[80];
  initializeBoard();
  
  printf("**************************************\n\r");
  printf("* ECE353: ICE-UART-Tx-IRQ DIST\n\r");
  printf("**************************************\n\r");
  
  // Infinite Loop
  while(1){
  memset(input,0,80);
  printf("\n\rEnter a string: ");
  scanf("%79[^\n]", input);
  printf("You entered: %s\n\r",input);
  };
}
