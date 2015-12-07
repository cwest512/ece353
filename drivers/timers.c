#include "timers.h"

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
