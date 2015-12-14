// Copyright (c) 2014, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "TM4C123GH6PM.h"
#include "board_util.h"
#include "driver_defines.h"
#include "gpio_port.h"
#include "i2c.h"
#include "wireless.h"
#include "adc.h"


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
// Initialize the I2C peripheral
//*****************************************************************************
void i2cInit(void)
{
	gpio_enable_port(EEPROM_GPIO_BASE);

	// Configure SCL
	gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN);
	gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN);
	gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN_PCTL); 

	// Configure SDA 
	gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN);
	gpio_config_open_drain(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN);
	gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN);
	gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN_PCTL);

  //  Initialize the I2C peripheral
  initializeI2CMaster(EEPROM_I2C_BASE);  
}

void adc_init(void)
{
		gpio_enable_port(PS2_GPIO_BASE);
		
		gpio_config_enable_input(PS2_GPIO_BASE, PS2_X_DIR_MASK);
		gpio_config_analog_enable(PS2_GPIO_BASE, PS2_X_DIR_MASK);
		gpio_config_alternate_function(PS2_GPIO_BASE, PS2_X_DIR_MASK);
		
		gpio_config_enable_input(PS2_GPIO_BASE, PS2_Y_DIR_MASK);
		gpio_config_analog_enable(PS2_GPIO_BASE, PS2_Y_DIR_MASK);
		gpio_config_alternate_function(PS2_GPIO_BASE, PS2_Y_DIR_MASK);
	
		initializeADC(PS2_ADC_BASE);
}

void pushButton_init(void)
{
	gpio_enable_port(DIR_BTN_BASE);
	gpio_enable_port(PS2_BTN_BASE);
	
  gpio_config_digital_enable(DIR_BTN_BASE, DIR_BTN_UP);
	gpio_config_enable_input(DIR_BTN_BASE, DIR_BTN_UP);
	gpio_config_enable_pullup(DIR_BTN_BASE, DIR_BTN_UP);
	
  gpio_config_digital_enable(DIR_BTN_BASE, DIR_BTN_DOWN);
	gpio_config_enable_input(DIR_BTN_BASE, DIR_BTN_DOWN);
	gpio_config_enable_pullup(DIR_BTN_BASE, DIR_BTN_DOWN);
	
	gpio_config_digital_enable(DIR_BTN_BASE, DIR_BTN_LEFT);
	gpio_config_enable_input(DIR_BTN_BASE, DIR_BTN_LEFT);
	gpio_config_enable_pullup(DIR_BTN_BASE, DIR_BTN_LEFT);
	
	gpio_config_digital_enable(DIR_BTN_BASE, DIR_BTN_RIGHT);
	gpio_config_enable_input(DIR_BTN_BASE, DIR_BTN_RIGHT);
	gpio_config_enable_pullup(DIR_BTN_BASE, DIR_BTN_RIGHT);
	
	gpio_config_digital_enable(PS2_BTN_BASE, PS2_BTN);
	gpio_config_enable_input(PS2_BTN_BASE, PS2_BTN);
	gpio_config_enable_pullup(PS2_BTN_BASE, PS2_BTN);
}

void rfInit(void)
{  
  wireless_set_pin_config(
    RF_SPI_BASE,
    RF_PAYLOAD_SIZE,
    RF_CHANNEL,
    RF_CS_BASE,
    RF_CS_PIN,
    RF_CE_GPIO_BASE,
    RF_CE_PIN
  );
  
  gpio_enable_port(RF_GPIO_BASE);
  
  // Configure SPI CLK
  gpio_config_digital_enable(  RF_GPIO_BASE, RF_CLK_PIN);
  gpio_config_alternate_function(    RF_GPIO_BASE, RF_CLK_PIN);
  gpio_config_port_control(     RF_GPIO_BASE, RF_CLK_PIN_PCTL);
  
  // Configure SPI MISO
  gpio_config_digital_enable(  RF_GPIO_BASE, RF_MISO_PIN);
  gpio_config_alternate_function(    RF_GPIO_BASE, RF_MISO_PIN);
  gpio_config_port_control(     RF_GPIO_BASE, RF_MISO_PIN_PCTL);
  
  // Configure SPI MOSI
  gpio_config_digital_enable(  RF_GPIO_BASE, RF_MOSI_PIN);
  gpio_config_alternate_function(    RF_GPIO_BASE, RF_MOSI_PIN);
  gpio_config_port_control(     RF_GPIO_BASE, RF_MOSI_PIN_PCTL);
  
  // Configure CS to be a normal GPIO pin that is controlled 
  // explicitly by software
  gpio_enable_port(RF_CS_BASE);
  gpio_config_digital_enable(  RF_CS_BASE,RF_CS_PIN);
  gpio_config_enable_output(    RF_CS_BASE,RF_CS_PIN);
  
  // Configure CE Pin as an output  
  gpio_enable_port(RF_CE_GPIO_BASE);
  gpio_config_digital_enable(  RF_CE_GPIO_BASE,RF_CE_PIN);
  gpio_config_enable_output(    RF_CE_GPIO_BASE,RF_CE_PIN);
	
//	gpio_enable_port(RF_IRQ_GPIO_BASE);
//	gpio_config_digital_enable(RF_IRQ_GPIO_BASE, RF_IRQ_PIN);
//	gpio_config_alternate_function(    RF_IRQ_GPIO_BASE, RF_IRQ_PIN);
//	gpio_config_port_control(     RF_IRQ_GPIO_BASE, GPIO_PCTL_PD7_M);
	
	//gpio_enable_interrupt(RF_GPIO_BASE,false);
	//NVIC_EnableIRQ(GPIOD_IRQn);
  initialize_spi( RF_SPI_BASE, 0, 10);
  RF_CE_PERIH->DATA |= (1 << 1);
}

void test_portD_interrupt(void)
{
	gpio_enable_port(RF_GPIO_BASE);
	
	gpio_config_digital_enable(RF_IRQ_GPIO_BASE, RF_IRQ_PIN);
	gpio_config_enable_input(RF_IRQ_GPIO_BASE, RF_IRQ_PIN);
	//gpio_enable_interrupt(RF_IRQ_GPIO_BASE,false);
}
	

void uart0_config_gpio(void)
{
	uint8_t pins = PA0 | PA1;
	uint8_t out = PA1;
	uint8_t in = PA0;
	
	if(!gpio_enable_port(GPIOA_BASE))
	{
		while(1){}
	}
	
	if(!gpio_config_digital_enable(GPIOA_BASE,pins))
	{
		while(1){}
	}
	
	if(!gpio_config_alternate_function(GPIOA_BASE,pins))
	{
		while(1){}
	}
	
	if(!gpio_config_uart(GPIOA_BASE))
	{
		while(1){}
	}
}
