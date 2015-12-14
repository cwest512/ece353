#include "pc_buffer.h"
#include "driver_defines.h"
#include "interrupts.h"
#include "timers.h"
#include "board_util.h"

volatile bool AlertSysTick;
volatile bool AlertFiveSec;
extern PC_Buffer UART0_Rx_Buffer;
extern PC_Buffer UART0_Tx_Buffer;
volatile int count = 0;
volatile bool updateXY;
volatile uint32_t ps2_x_data, ps2_y_data;
ADC0_Type* myADC = (ADC0_Type *)PS2_ADC_BASE;
volatile bool readIn = true;

//*****************************************************************************
// Rx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART0_Rx_Flow(PC_Buffer *rx_buffer)
{

	  // Loop until all characters in the RX FIFO have been removed
	while((UART0->FR & UART_FR_RXFE) == 0)
	{
      // Inside Loop: Add the character to the circular buffer
			pc_buffer_add(rx_buffer, UART0->DR);
	}
  // Clear the RX interrupts so it can trigger again when the hardware 
  // FIFO becomes full
	UART0->ICR |= UART_ICR_RXIC ;
	UART0->ICR |= UART_ICR_RTIC ;
}

//*****************************************************************************
// Tx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART0_Tx_Flow(PC_Buffer *tx_buffer)
{
      char c;
	
        // Check to see if we have any data in the circular queue
				while(!pc_buffer_empty(tx_buffer))
				{
            // Move data from the circular queue to the hardware FIFO
            // until the hardware FIFO is full or the circular buffer
            // is empty.
						pc_buffer_remove(tx_buffer, &c);
						UART0->DR = c;
						if(UART0->FR & UART_FR_TXFF)
						{
							UART0->ICR |= UART_ICR_TXIC;
							return;
						}
					
				}

        // Clear the TX interrupt so it can trigger again when the hardware
        // FIFO is empty
				UART0->ICR |= UART_ICR_TXIC;
}

//*****************************************************************************
// UART0 Interrupt Service handler
//*****************************************************************************
void UART0_Handler(void)
{
    uint32_t  status;

    // Check to see if RXMIS or RTMIS is active
    status = UART0->MIS;

    if ( status & (UART_MIS_RXMIS | UART_MIS_RTMIS ) )
    {
       UART0_Rx_Flow(&UART0_Rx_Buffer);
    }

    // Check the TX interrupts
    if ( status & UART_MIS_TXMIS )
    {
      UART0_Tx_Flow(&UART0_Tx_Buffer);
    }
    return;
}

void SysTick_Handler(void) 
{
	uint32_t val;
	
	
	if(count == 9) {
		AlertSysTick = true;
	}
	else
		AlertSysTick = false;
	
	count = (count + 1) % 10;
	
	val = SysTick->VAL;
	
}
void TIMER0A_Handler(void)
{
	updateXY = true;
	a_timer->ICR = TIMER_ICR_TATOCINT;
}

void TIMER1A_Handler(void)
{	
  AlertFiveSec = true;
	one_timer->ICR = TIMER_ICR_TATOCINT;
}

void WDT0_Handler(void)
{
	printf("You didn't pet the dog!");
}

void GPIOD_Handler(void)
{  	
	if((GPIOD->RIS & ~0x80) == 0)
		{
		readIn = true;
	}
	GPIOD->ICR = 0;
}

void ADC0SS1_Handler(void)
{
	
	ps2_x_data = myADC->SSFIFO1 & 0xfff;
	ps2_y_data = myADC->SSFIFO1 & 0xfff;
	
	myADC->ISC = 0x2;
}