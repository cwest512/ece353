#include "pc_buffer.h"
#include "driver_defines.h"
#include "interrupts.h"
#include "timers.h"
#include "board_util.h"

// SysTick interrupt variable
volatile bool AlertSysTick;
// Alert Variable to be set every 5 seconds
volatile bool AlertFiveSec;
// Link to RX buffer
extern PC_Buffer UART0_Rx_Buffer;
// Link to TX Buffer
extern PC_Buffer UART0_Tx_Buffer;
// Count to keep track of the number of Systick interrupts
volatile int count = 0;
// Signal to initiate ADC conversion
volatile bool updateXY;
// Signal to send Speed data
volatile bool sendSpeed;
// Link to joystick data
volatile uint32_t ps2_x_data, ps2_y_data;
// Reference to Joystick mem location
ADC0_Type* myADC = (ADC0_Type *)PS2_ADC_BASE;
// Signal to indicate when to read from RF
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
//*****************************************************************************
// SysTick Interrupt Service handler
//*****************************************************************************
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

//*****************************************************************************
// Timer0A Interrupt Service handler
//*****************************************************************************
void TIMER0A_Handler(void)
{
	updateXY = true;
	a_timer->ICR = TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// Timer1A Interrupt Service handler
//*****************************************************************************
void TIMER1A_Handler(void)
{	
  AlertFiveSec = true;
	one_timer->ICR = TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// Watchdog Interrupt Service handler
//*****************************************************************************
void WDT0_Handler(void)
{
	printf("You didn't pet the dog!");
}

//*****************************************************************************
// GPIO Port D Interrupt Service handler
//*****************************************************************************
void GPIOD_Handler(void)
{
	readIn = true;
	petTheDog(500E6);
	GPIOD->ICR = 0x80;
}

//*****************************************************************************
// Analog-to-Digital Interrupt Service handler
//*****************************************************************************
void ADC0SS1_Handler(void)
{
	
	ps2_x_data = myADC->SSFIFO1 & 0xfff;
	ps2_y_data = myADC->SSFIFO1 & 0xfff;
	
	myADC->ISC = 0x2;
}

//*****************************************************************************
// Timer2A Interrupt Service handler
//*****************************************************************************
void TIMER2A_Handler(void)
{
	sendSpeed = true;
	two_timer->ICR = TIMER_ICR_TATOCINT;
}
