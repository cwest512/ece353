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
#include "boardUtil.h"
#include "../include/sysctrl.h"

#include "uart.h"
#include "eeprom.h"
#include "eadogs102w.h"

#include "pc_buffer.h"

#define UART0_BUFFER_SIZE 80


PC_Buffer UART0_Rx_Buffer;
char UART0_Rx_Buffer_Array[UART0_BUFFER_SIZE];

PC_Buffer UART0_Tx_Buffer;
char UART0_Tx_Buffer_Array[UART0_BUFFER_SIZE];



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
// Configure PA0 and PA1 to be UART pins
//*****************************************************************************
void uart0_config_gpio(void)
{
   gpio_enable_port(GPIOA_BASE);
   gpio_config_digital_enable( GPIOA_BASE, PA0 | PA1);
   gpio_config_alternate_function( GPIOA_BASE, PA0 | PA1);
   gpio_config_port_control( GPIOA_BASE, GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);
}

//*****************************************************************************
//*****************************************************************************
void serialDebugInit(void)
{
 // Configure GPIO Pins
  uart0_config_gpio();
  
  // Configure Circular Buffers
  pc_buffer_init(&UART0_Rx_Buffer , UART0_Rx_Buffer_Array, UART0_BUFFER_SIZE); 
  pc_buffer_init(&UART0_Tx_Buffer , UART0_Tx_Buffer_Array, UART0_BUFFER_SIZE); 
  
  // Initialize UART0 for 8N1, interrupts enabled.
  uart_init_115K(
    UART0_BASE, 
    SYSCTL_RCGCUART_R0, 
    SYSCTL_PRUART_R0, 
    UART0_IRQn,
    1
  );
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


//*****************************************************************************
// Initialize the LCD peripheral
//*****************************************************************************
void lcd_init(void)
{
  
  // Configure SPI CLK
  gpio_enable_port(LCD_GPIO_BASE);
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_CLK_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_CLK_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_CLK_PIN_PCTL);
    
  // Configure SPI CS
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_CS_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_CS_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_CS_PIN_PCTL);

  // Configure SPI MOSI
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_MOSI_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_MOSI_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_MOSI_PIN_PCTL);
  
  // Configure CD
  gpio_enable_port(GPIO_LCD_CD_BASE);
  gpio_config_digital_enable(GPIO_LCD_CD_BASE,LCD_CD_PIN);
  gpio_config_enable_output(GPIO_LCD_CD_BASE,LCD_CD_PIN);
  
  // Configure RST_N
  gpio_enable_port(GPIO_LCD_RST_N_BASE);
  gpio_config_digital_enable(GPIO_LCD_RST_N_BASE, LCD_RST_N_PIN);
  gpio_config_enable_output(GPIO_LCD_RST_N_BASE, LCD_RST_N_PIN);
  
  lcd_set_pin_config(LCD_SPI_BASE,GPIO_LCD_RST_N_BASE,LCD_RST_N_PIN,GPIO_LCD_CD_BASE,LCD_CD_PIN);
  
  initialize_spi( LCD_SPI_BASE, 3, 2);
  lcd_initialize();
  
}


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

//*****************************************************************************
//*****************************************************************************
void ps2Init(void)
{
   // Configure the X direction of the PS2 as an analog input
  gpio_enable_port(PS2_X_DIR_BASE);
  gpio_config_enable_input(PS2_X_DIR_BASE,   PS2_X_DIR_PIN);
  gpio_config_analog_enable(PS2_X_DIR_BASE, PS2_X_DIR_PIN);
  gpio_config_alternate_function(PS2_X_DIR_BASE,  PS2_X_DIR_PIN);
  
  // Configure the Y direction of the PS2 as an analog input
  gpio_enable_port(PS2_Y_DIR_BASE);
  gpio_config_enable_input(PS2_Y_DIR_BASE,   PS2_Y_DIR_PIN);
  gpio_config_analog_enable(PS2_Y_DIR_BASE, PS2_Y_DIR_PIN);
  gpio_config_alternate_function(PS2_Y_DIR_BASE,  PS2_Y_DIR_PIN);
  
  // Configure the button
  gpio_enable_port(PS2_BTN_BASE);
  gpio_config_digital_enable(PS2_BTN_BASE,PS2_BTN_PIN);
  gpio_config_enable_input(PS2_BTN_BASE,PS2_BTN_PIN);
  gpio_config_enable_pullup( PS2_BTN_BASE,PS2_BTN_PIN); 
}

//*****************************************************************************
//*****************************************************************************
void pushButtonsInit(void)
{
  
  gpio_enable_port(BTN_UP_BASE);
  gpio_config_digital_enable(BTN_UP_BASE,BTN_UP_PIN);
  gpio_config_enable_input(BTN_UP_BASE,BTN_UP_PIN);
  gpio_config_enable_pullup( BTN_UP_BASE,BTN_UP_PIN); 
  
  gpio_enable_port(BTN_RIGHT_BASE);
  gpio_config_digital_enable(BTN_RIGHT_BASE,BTN_RIGHT_PIN);
  gpio_config_enable_input(BTN_RIGHT_BASE,BTN_RIGHT_PIN);
  gpio_config_enable_pullup( BTN_RIGHT_BASE,BTN_RIGHT_PIN); 
  

  gpio_enable_port(BTN_LEFT_BASE);
  gpio_config_digital_enable(BTN_LEFT_BASE,BTN_LEFT_PIN);
  gpio_config_enable_input(BTN_LEFT_BASE,BTN_LEFT_PIN);
  gpio_config_enable_pullup( BTN_LEFT_BASE,BTN_LEFT_PIN); 
  
  gpio_enable_port(BTN_DOWN_BASE);
  gpio_config_digital_enable(BTN_DOWN_BASE,BTN_DOWN_PIN);
  gpio_config_enable_input(BTN_DOWN_BASE,BTN_DOWN_PIN);
  gpio_config_enable_pullup( BTN_DOWN_BASE,BTN_DOWN_PIN); 
  
}

//*****************************************************************************
// Set new wireless IDs
//*****************************************************************************
void setWirelessIDs(uint8_t *myID, uint8_t *remoteID)
{
  
  char msg[2];
  
  printf("Enter two byte hexidecimal number for 353 Controller: 0x353");
  scanf("%s",msg);
  myID[0] = '3';
  myID[1] = '5';
  myID[2] = '3';
  myID[3] = msg[0];
  myID[4] = msg[1];

  eeprom_byte_write(EEPROM_I2C_BASE,0, myID[0]);
  eeprom_byte_write(EEPROM_I2C_BASE,1, myID[1]);
  eeprom_byte_write(EEPROM_I2C_BASE,2, myID[2]);
  eeprom_byte_write(EEPROM_I2C_BASE,3, myID[3]);
  eeprom_byte_write(EEPROM_I2C_BASE,4, myID[4]);

  printf("Enter two byte hexidecimal number for the 315 Robot: 0x353");
  scanf("%s",msg);
  remoteID[0] = '3';
  remoteID[1] = '5';
  remoteID[2] = '3';
  remoteID[3] = msg[0];
  remoteID[4] = msg[1];
  eeprom_byte_write(EEPROM_I2C_BASE,5, remoteID[0]);
  eeprom_byte_write(EEPROM_I2C_BASE,6, remoteID[1]);
  eeprom_byte_write(EEPROM_I2C_BASE,7, remoteID[2]);
  eeprom_byte_write(EEPROM_I2C_BASE,8, remoteID[3]);
  eeprom_byte_write(EEPROM_I2C_BASE,9, remoteID[4]);
  wireless_configure_device(myID, remoteID ) ;
  
  printf("STAFF   ID: %s\n\r", myID);
  printf("STUDENT ID: %s\n\r", remoteID);
  
}
  
//*****************************************************************************
// Read in and set the wirless IDs
//*****************************************************************************
void getWirelessIDs(uint8_t *myID, uint8_t *remoteID)
{
  char msg[2];
  // Read in the device IDs
  eeprom_byte_read(EEPROM_I2C_BASE,0, &myID[0]);
  eeprom_byte_read(EEPROM_I2C_BASE,1, &myID[1]);
  eeprom_byte_read(EEPROM_I2C_BASE,2, &myID[2]);
  eeprom_byte_read(EEPROM_I2C_BASE,3, &myID[3]);
  eeprom_byte_read(EEPROM_I2C_BASE,4, &myID[4]);
  
  if( myID[0] != '3' ||  myID[1] != '5' || myID[2] != '3')
  {
    printf("Enter two byte hexidecimal number for this board: 0x353");
    scanf("%s",msg);
    myID[0] = '3';
    myID[1] = '5';
    myID[2] = '3';
    myID[3] = msg[0];
    myID[4] = msg[1];

    eeprom_byte_write(EEPROM_I2C_BASE,0, myID[0]);
    eeprom_byte_write(EEPROM_I2C_BASE,1, myID[1]);
    eeprom_byte_write(EEPROM_I2C_BASE,2, myID[2]);
    eeprom_byte_write(EEPROM_I2C_BASE,3, myID[3]);
    eeprom_byte_write(EEPROM_I2C_BASE,4, myID[4]);
  }
  
  eeprom_byte_read(EEPROM_I2C_BASE,5, &remoteID[0]);
  eeprom_byte_read(EEPROM_I2C_BASE,6, &remoteID[1]);
  eeprom_byte_read(EEPROM_I2C_BASE,7, &remoteID[2]);
  eeprom_byte_read(EEPROM_I2C_BASE,8, &remoteID[3]);
  eeprom_byte_read(EEPROM_I2C_BASE,9, &remoteID[4]);
  
  if( remoteID[0] != '3' ||  remoteID[1] != '5' || remoteID[2] != '3')
  {
    printf("Enter two byte hexidecimal number for remote board: 0x353");
    scanf("%s",msg);
    remoteID[0] = '3';
    remoteID[1] = '5';
    remoteID[2] = '3';
    remoteID[3] = msg[0];
    remoteID[4] = msg[1];
    eeprom_byte_write(EEPROM_I2C_BASE,5, remoteID[0]);
    eeprom_byte_write(EEPROM_I2C_BASE,6, remoteID[1]);
    eeprom_byte_write(EEPROM_I2C_BASE,7, remoteID[2]);
    eeprom_byte_write(EEPROM_I2C_BASE,8, remoteID[3]);
    eeprom_byte_write(EEPROM_I2C_BASE,9, remoteID[4]);
  }
  //wireless_configure_device(myID, remoteID ) ;
  printf("STAFF   ID: %s\n\r", myID);
  printf("STUDENT ID: %s\n\r", remoteID);
  
}

//*****************************************************************************
//*****************************************************************************
bool checkButtons(uint32_t base, uint8_t pin)
{
  static uint16_t btnUp     = 0xFFFF;
  static uint16_t btnDown   = 0xFFFF;
  static uint16_t btnLeft   = 0xFFFF;
  static uint16_t btnRight  = 0xFFFF;
  
  GPIOA_Type *myGpio = (GPIOA_Type *)base;
  
  switch(pin)
  {
    case BTN_UP_PIN:
    {
       btnUp = btnUp << 1;
       if( (myGpio->DATA & pin) != 0)
       {
         btnUp |= 0x01;
       }
       
       if (btnUp == 0x8000)
         return true;
       else
         return false;
    }
        
     case BTN_DOWN_PIN:
    {
       btnDown = btnDown << 1;
       if( (myGpio->DATA & pin) != 0)
       {
         btnDown |= 0x01;
       }
       
       if (btnDown == 0x8000)
         return true;
       else
         return false;
    }
        
    case BTN_LEFT_PIN:
    {
       btnLeft = btnLeft << 1;
       if( (myGpio->DATA & pin) != 0)
       {
         btnLeft |= 0x01;
       }
       
       if (btnLeft == 0x8000)
         return true;
       else
         return false;
    }
    
    case BTN_RIGHT_PIN:
    {
       btnRight = btnRight << 1;
       if( (myGpio->DATA & pin) != 0)
       {
         btnRight |= 0x01;
       }
       
       if (btnRight == 0x8000)
         return true;
       else
         return false;
       
    }
    default:
    {
      return false;
    }
    
  }
}

//*****************************************************************************
//*****************************************************************************
void gpTimersInit(void)
{
   GPTIMER_CONFIG timer0_config;
   GPTIMER_CONFIG timer1_config;
  
  
   // Configure the timers used to debounce push buttons
   // and used for waiting x miliseconds.
   timer0_config.base = TIMER0_BASE;
   timer0_config.mode = TIMER_CFG_16_BIT;
   // Configure Sub timer A
   timer0_config.enableA = true;
   timer0_config.countA = TEN_MS;
   timer0_config.countModeA = TIMER_TAMR_TAMR_PERIOD;
   timer0_config.priorityA = 0;
   timer0_config.intEnA = true;
   timer0_config.irqNumA = TIMER0A_IRQn;
   // Configure Sub Timer B
   timer0_config.enableB = true;
   timer0_config.countB = TEN_MS;
   timer0_config.countModeB = TIMER_TBMR_TBMR_1_SHOT;
   timer0_config.priorityB = 0;
   timer0_config.intEnB = true;
   timer0_config.irqNumB = TIMER0B_IRQn; 
  

   // Set the timer that is used to determine when the ADC
   // will be sampled.
   timer1_config.base = TIMER1_BASE;
   timer1_config.mode = TIMER_CFG_32_BIT_TIMER;
   // Configure Sub timer A
   timer1_config.enableA = true;
   timer1_config.countA = ONE_SEC;
   timer1_config.countModeA = TIMER_TAMR_TAMR_PERIOD;
   timer1_config.priorityA = 0;
   timer1_config.intEnA = true;
   timer1_config.irqNumA = TIMER1A_IRQn; 
   // Configure Sub timer B
   timer1_config.enableB = false;
   timer1_config.countB = 0;
   timer1_config.countModeB = 0;
   timer1_config.priorityB = 0;
   timer1_config.intEnB = false;
   timer1_config.irqNumB = TIMER1B_IRQn; 
  
   // Enabled the timers.
   gp_timer_config(&timer0_config);
   gp_timer_config(&timer1_config);
}
