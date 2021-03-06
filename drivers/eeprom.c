#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "eeprom.h"
#include "board_util.h"
#include "timers.h"

//*****************************************************************************
// Used to determine if the EEPROM is busy writing the last transaction to 
// non-volatile storage
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
// Returns
// I2C_OK is returned one the EEPROM is ready to write the next byte
//*****************************************************************************
static 
i2c_status_t eeprom_wait_for_write( int32_t  i2c_base)
{
  
  i2c_status_t status;
  
  if( !i2cVerifyBaseAddr(i2c_base) )
  {
    return  I2C_INVALID_BASE;
  }

  // Set the I2C address to be the EEPROM and in Write Mode
  status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE);

  // Poll while the device is busy.  The  MCP24LC32AT will not ACK
  // writing an address while the write has not finished.
  do 
  {
    // The data we send does not matter.  This has been set to 0x00, but could
    // be set to anything
    status = i2cSendByte( i2c_base, 0x00, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
    
    // Wait for the address to finish transmitting
    while ( I2CMasterBusy(i2c_base)) {};
    
    // If the address was not ACKed, try again.
  } while (I2CMasterAdrAck(i2c_base) == false);

  return  status;
}
  
  
//*****************************************************************************
// Writes a single byte of data out to the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       Data written to the EEPROM.
//
// Returns
// I2C_OK if the byte was written to the EEPROM.
//*****************************************************************************
i2c_status_t eeprom_byte_write
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // Set the I2C address to be the EEPROM
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE); 
  
  // If the EEPROM is still writing the last byte written, wait
  eeprom_wait_for_write(i2c_base);
  
  //==============================================================
  // Send the Upper byte of the address
	// ADD CODE	
  //==============================================================
	i2cSendByte(
     i2c_base,
     (address & 0xff00) >> 4 ,
     I2C_MCS_START | I2C_MCS_RUN
	);
		
  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================
	i2cSendByte(
     i2c_base,
     address & 0xff,
     I2C_MCS_RUN
	);

  //==============================================================
  // Send the Byte of data to write
	// ADD CODE
  //==============================================================
	i2cSendByte(
     i2c_base,
     data,
     I2C_MCS_RUN | I2C_MCS_STOP
		);
		
  return status;
}

//*****************************************************************************
// Reads a single byte of data from the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       data read from the EEPROM is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the EEPROM.
//*****************************************************************************
i2c_status_t eeprom_byte_read
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   *data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  // If the EEPROM is still writing the last byte written, wait
  eeprom_wait_for_write(i2c_base);

  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Write Mode
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE);

  //==============================================================
  // Send the Upper byte of the address
	// ADD CODE
  //==============================================================
	i2cSendByte(
     i2c_base,
     (address & 0xff00) >> 4 ,
     I2C_MCS_START | I2C_MCS_RUN
	);

  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================
	i2cSendByte(
     i2c_base,
     address & 0xff,
     I2C_MCS_RUN
	);

  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Read Mode
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_READ);

  //==============================================================
  // Read the data returned by the EEPROM
	// ADD CODE
  //==============================================================
  i2cGetByte(
     i2c_base,
     data,
     I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP
	);
  return I2C_OK;
}

//*****************************************************************************
// Modifies the eeprom contents and then resets the board
//*****************************************************************************
void edit_eeprom(void)
{
	//input char is 80 bytes long
	char input[80];
	//increment var
	int j;
	//reading from eeprom var
	uint8_t toRead[80];
		
	//Turn off WatchDog Timer
	wd_timer = (WATCHDOG0_Type *) WATCHDOG0_BASE;
	NVIC_DisableIRQ(WATCHDOG0_IRQn);

	//Prompt to modify eeprom contents
	printf("\nModify EEPROM Contents");
	
	//Student One
	printf("\nStudent 1: ");
	
	//Read current student 1 value in eeprom
	for(j = 0; j < 80; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j, &toRead[j] );
	
	//print it out
	for(j = 0; j < 80; j++)
		printf("%c", toRead[j]);
		
	//prompt ofr new student 1
	printf("\n\rEnter a new Student 1: ");
	//sets input to 80 and start writing at 0
	memset(input,0,80);
	//look for 80 input
	scanf("%80[^\n]", input);
	
	//if Enter was not used then write to eeprom, otherwise skip
	if(!(input[0] == NULL))
	{
		for(j = 0; j < 80; j++)
			eeprom_byte_write(EEPROM_I2C_BASE, j, input[j]);
	}
	
	//Student 2
	printf("\nStudent 2: ");
	
	//Read Student 2 from eeprom
	for(j = 0; j < 80; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j+80, &toRead[j] );
		
	//Print out
	for(j = 0; j < 80; j++)
		printf("%c", toRead[j]);
		
	//Prompt for new student 2
	printf("\n\rEnter a new Student 2: ");
	memset(input,0,80);
	scanf("%80[^\n]", input);
	
	//if Enter was not used then write to eeprom, otherwise skip
	if(!(input[0] == NULL))
	{
		for(j = 0; j < 80; j++)
			eeprom_byte_write(EEPROM_I2C_BASE, j+80, input[j]);
	}
		
	//Group #
	printf("\nTeam Number: ");
	
	//Read Group # from EEPROM
	for(j = 0; j < 2; j++)
		eeprom_byte_read(EEPROM_I2C_BASE, j+160, &toRead[j] );
		
	//Print out
	for(j = 0; j < 2; j++)
		printf("%c", toRead[j]);
		
	//Prompt for new eeprom
	printf("\n\rEnter a new Team Number: ");
	memset(input,0,2);
	//2 bytes long input
	scanf("%2[^\n]", input);
	
	//if Enter was not used then write to eeprom, otherwise skip
	if(!(input[0] == NULL))
	{
		for(j = 0; j < 2; j++)
			eeprom_byte_write(EEPROM_I2C_BASE, j+160, input[j]);
	}

	//SOFTWARE RESET
	NVIC_SystemReset();
}

