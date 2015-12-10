// Copyright (c) 2014-2015, Joe Krachey
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
#include "board_setup.h"

//*****************************************************************************
//*****************************************************************************
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

  initialize_spi( RF_SPI_BASE, 0, 10);
  RF_CE_PERIH->DATA |= (1 << 1);
}

