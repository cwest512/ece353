#include "adc.h"

/******************************************************************************
 * Initializes ADC to use Sample Sequencer #3, triggered by the processor,
 * no IRQs
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
      
      // ADD CODE
      // set rcgc_adc_mask
      rcgc_adc_mask = SYSCTL_RCGCADC_R0;
      // Set pr_mask 
			pr_mask = SYSCTL_PRADC_R0;
      break;
    }
    case ADC1_BASE :
    {
        // ADD CODE
      // set rcgc_adc_mask
      rcgc_adc_mask = SYSCTL_RCGCADC_R1;
      // Set pr_mask 
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
  
  // ADD CODE
  // disable sample sequencer #1 by writing a 0 to the 
  // corresponding ASENn bit in the ADCACTSS register 
		
		myADC->IM = 0;
		
	myADC->ACTSS &= ~ADC_ACTSS_ASEN1;

  // ADD CODE
  // Set the event multiplexer to trigger conversion on a processor trigger
  // for sample sequencer #1.
		
	myADC->EMUX  &= ~ADC_EMUX_EM1_M;
	myADC->EMUX  |= ADC_EMUX_EM1_PROCESSOR;

  // ADD CODE
  // Set IE0 and END0 in SSCTL3
  myADC->SSCTL1 = ADC_SSCTL1_IE1 | ADC_SSCTL1_END1;
	NVIC_EnableIRQ(ADC0SS1_IRQn);
		myADC->IM = 2;
	
		
  return true;
}

/******************************************************************************
 * Reads SSMUX3 for the given ADC.  Busy waits until completion
 *****************************************************************************/
uint32_t getADCValue( uint32_t adc_base, uint8_t channel)
{
  ADC0_Type  *myADC;
  uint32_t result;
  
  if( adc_base == 0)
  {
    return false;
  }
  
  myADC = (ADC0_Type *)adc_base;
  
  myADC->SSMUX1 = channel;          // Set the Channel
  
  myADC->ACTSS |= ADC_ACTSS_ASEN1;  // Enable SS3
  
  myADC->PSSI =   ADC_PSSI_SS1;     // Start SS3
  
  while( (myADC->RIS & ADC_RIS_INR1)  == 0)
  {
    // wait
  }
  
  result = myADC->SSFIFO1 & 0xFFF;    // Read 12-bit data
  
  myADC->ISC  = ADC_ISC_IN1;          // Ack the conversion
  
  return result;
}

