#include "board_config.h"
#include "gpio_port.h"

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
  gpio_enable_port(GPIOA_BASE);
	gpio_config_digital_enable(GPIOA_BASE, PA1|PA0);
	gpio_config_alternate_function(GPIOA_BASE, PA1|PA0);
	gpio_config_port_control(GPIOA_BASE, (GPIO_PCTL_PA1_U0TX|GPIO_PCTL_PA0_U0RX));
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


