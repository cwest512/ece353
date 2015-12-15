#include "adc.h"

/******************************************************************************
 * Initializes ADC to use Sample Sequencer #1, triggered by the timer,
 * no IRQs
 * 
 * RETURNS
 * 	Boolean value to signify successful completion
 *****************************************************************************/
bool initializeADC(  uint32_t adc_base )
{
  ADC0_Type  *myADC;
  uint32_t rcgc_adc_mask;
  uint32_t pr_mask;
  

  // examine the adc_base.  Verify that it is either ADC0 or ADC1
  // Set the rcgc_adc_mask and pr_mask  
  switch (adc_base) 
  {
    case ADC0_BASE :
    {
      
      
      // Set gate clock mask
      rcgc_adc_mask = SYSCTL_RCGCADC_R0;
      // Set peripheral ready
			pr_mask = SYSCTL_PRADC_R0;
      break;
    }
    case ADC1_BASE :
    {
      // Set gate clock mask
      rcgc_adc_mask = SYSCTL_RCGCADC_R1;
      // Set "peripheral ready" mask
			pr_mask = SYSCTL_PRADC_R1;
      break;
    }
    
    default:
      return false;
  }
  
  // Turn on the ADC Clock
  SYSCTL->RCGCADC |= rcgc_adc_mask;
  
  // Wait for ADCx to become ready
  while( (pr_mask & SYSCTL->PRADC) != pr_mask){}
    
  // Type Cast adc_base and set it to myADC
  myADC = (ADC0_Type *)adc_base;
  
  // Disable sample sequencer #1 by writing a 0 to the 
  // Corresponding ASENn bit in the ADCACTSS register 
	
	myADC->IM = 0;
		
	myADC->ACTSS &= ~ADC_ACTSS_ASEN1;

  // Set the event multiplexer to trigger conversion on a timer trigger
  // for sample sequencer #1.
		
	myADC->EMUX  &= ~ADC_EMUX_EM1_M;
	myADC->EMUX  |= ADC_EMUX_EM1_TIMER;
		
	myADC->SSMUX1 = 0x01;
	
  // Set 2nd conversion as the end-of-sequence
  myADC->SSCTL1 = ADC_SSCTL1_IE1 | ADC_SSCTL1_END1;
	
	// Unmask interrupts for SSI1
	myADC->IM = 0x2;
	// Register SSI1 with Interrupt Controller
	NVIC_EnableIRQ(ADC0SS1_IRQn);
	
	// Enable ADC conversions
	myADC->ACTSS |= ADC_ACTSS_ASEN1;
	//Initiate Sampling
	myADC->PSSI 	= ADC_PSSI_SS1;
	
  return true;
}


/******************************************************************************
 * Reads SSMUX3 for the given ADC.  Busy waits until completion
 *****************************************************************************/
//uint32_t getADCValue( uint32_t adc_base, uint8_t channel)
//{
//  ADC0_Type  *myADC;
//  uint32_t result;
//  
//  if( adc_base == 0)
//  {
//    return false;
//  }
//  
//  myADC = (ADC0_Type *)adc_base;
//  
//  myADC->SSMUX3 = channel;          // Set the Channel
//  
//  myADC->ACTSS |= ADC_ACTSS_ASEN3;  // Enable SS3
//  
//  myADC->PSSI =   ADC_PSSI_SS3;     // Start SS3
//  
//  while( (myADC->RIS & ADC_RIS_INR3)  == 0)
//  {
//    // wait
//  }
//  
//  result = myADC->SSFIFO3 & 0xFFF;    // Read 12-bit data
//  
//  myADC->ISC  = ADC_ISC_IN3;          // Ack the conversion
//  
//  return result;
//}

