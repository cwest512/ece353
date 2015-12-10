#include "timers.h"

volatile TIMER0_Type *a_timer;
volatile TIMER0_Type *one_timer;
volatile WATCHDOG0_Type *wd_timer;

//*****************************************************************************
// Configure a 16/32 bit general purpose timer to wait a specified number
// of clock cycles
//*****************************************************************************
bool gp_timer_wait(uint32_t base, uint32_t ticks)
{
  TIMER0_Type *gp_timer;
  uint32_t timer_rcgc_mask;
  uint32_t timer_pr_mask;
  
  // Set the timer_rcgc_mask and timer_pr_mask using the appropriate
  // #defines in ../include/sysctrl.h
  switch(base)
  {
    case TIMER0_BASE:
    {
      timer_rcgc_mask = SYSCTL_RCGCTIMER_R0;
      timer_pr_mask = SYSCTL_PRTIMER_R0;
      break;
    }
    case TIMER1_BASE:
    {
      timer_rcgc_mask = SYSCTL_RCGCTIMER_R1;
      timer_pr_mask = SYSCTL_PRTIMER_R1;
      break;
    }
    case TIMER2_BASE:
    {
      timer_rcgc_mask = SYSCTL_RCGCTIMER_R2;
      timer_pr_mask = SYSCTL_PRTIMER_R2;
      break;
    }
    case TIMER3_BASE:
    {
      timer_rcgc_mask = SYSCTL_RCGCTIMER_R3;
      timer_pr_mask = SYSCTL_PRTIMER_R3;
      break;
    }
    case TIMER4_BASE:
    {
      timer_rcgc_mask = SYSCTL_RCGCTIMER_R4;
      timer_pr_mask = SYSCTL_PRTIMER_R4;
      break;
    }
    case TIMER5_BASE:
    {
      timer_rcgc_mask = SYSCTL_RCGCTIMER_R5;
      timer_pr_mask = SYSCTL_PRTIMER_R5;
      break;
    }
    default:
    {
      return false;
    }
  }
  
  // Turn on the clock for the timer
  SYSCTL->RCGCTIMER |= timer_rcgc_mask;

  // Wait for the timer to turn on
  while( (SYSCTL->PRTIMER & timer_pr_mask) == 0) {};
    
  
  // ADD CODE
  // Initialize the timer to be a 
  //      32-bit
  //      one-shot
  //      count down
  //      no interrupts
  //
  // See the initialization steps provided in the ECE353 eBook or
  // the data sheet.
  //
  // http://ece353.engr.wisc.edu/ece353-the-book/timers/
		gp_timer = (TIMER0_Type *) base;
		gp_timer->CTL = 0x0;
		gp_timer->CFG = TIMER_CFG_32_BIT_TIMER;
		gp_timer->TAMR = TIMER_TAMR_TAMR_1_SHOT;
		gp_timer->TAILR = ticks;
		gp_timer->ICR |= TIMER_ICR_TATOCINT;
		gp_timer->CTL |= TIMER_CTL_TAEN;
		
		while( (gp_timer->RIS & TIMER_RIS_TATORIS) == 0){}
		
  return true;
}
void timer0_configA(uint16_t ticks)
{
		// Turn on the clock for the timer
		SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R0;
		// Wait for the timer to turn on
		while( (SYSCTL->PRTIMER & SYSCTL_PRTIMER_R0) == 0) {};
		//Cast address to TIMER0_Type pointer
		a_timer = (TIMER0_Type *) TIMER0_BASE;
		//Disable timerA
		a_timer->CTL &= ~(TIMER_CTL_TAEN);
		//Set timer as 32 bit
		a_timer->CFG = TIMER_CFG_32_BIT_TIMER;
		//Set timer as a count-down and periodic
		a_timer->TAMR = TIMER_TAMR_TAMR_PERIOD;
		//Set the number of clock cycles (ticks) to count down from
		a_timer->TAILR = ticks;
		//Clear TimerA interrupt
		a_timer->ICR |= TIMER_ICR_TATOCINT;	
		//Enable interrupts by setting the mask and registering with the NVIC
		a_timer->IMR |= TIMER_IMR_TATOIM;
		NVIC_EnableIRQ(TIMER0A_IRQn);
}

void timer1_configA(uint32_t ticks)
{
		// Turn on the clock for the timer
		SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R1;
		// Wait for the timer to turn on
		while( (SYSCTL->PRTIMER & SYSCTL_PRTIMER_R1) == 0) {};
		//Cast address to TIMER0_Type pointer
		one_timer = (TIMER0_Type *) TIMER1_BASE;
		//Disable timerA
		one_timer->CTL &= ~(TIMER_CTL_TAEN);
		//Set timer as 32 bit
		one_timer->CFG = TIMER_CFG_32_BIT_TIMER;
		//Set timer as a count-down and periodic
		one_timer->TAMR = TIMER_TAMR_TAMR_PERIOD;
		//Set the number of clock cycles (ticks) to count down from
		one_timer->TAILR = ticks;
		//Clear TimerA interrupt
		one_timer->ICR |= TIMER_ICR_TATOCINT;	
		//Enable interrupts by setting the mask and registering with the NVIC
		one_timer->IMR |= TIMER_IMR_TATOIM;
		NVIC_EnableIRQ(TIMER1A_IRQn);
}

void watchdog_config(uint32_t ticks)
{
//		SYSCTL->RCGCWD |= SYSCTL_RCGCWD_R0;
//		while( (SYSCTL->PRWD & SYSCTL_PRWD_R0) == 0) {};
//			
//		wd_timer = (WATCHDOG0_Type *) WTIMER0_BASE;
//			
//		wd_timer->LOAD = ticks;	
//			
//		// Set RESEN bit to trigger hardware reset
//		//wd_timer->CTL |= 0x02;
//		
//		//Set INTEN to register and enable the watchdog
//		
//		wd_timer->CTL = 0x01;
	
}
