#include "board_config.h"

extern void uart0_config_gpio(void);

void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}


//*****************************************************************************
//*****************************************************************************
static void serialDebugInit(void)
{
  uart0_config_gpio();
  initialize_uart();
}

//*****************************************************************************
//*****************************************************************************
void initializeBoard(void)
{
  
  DisableInterrupts();
  serialDebugInit();
  EnableInterrupts();
   
}


