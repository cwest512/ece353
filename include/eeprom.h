#ifndef __EEPROM_H__
#define __EEPROM_H__


#include <stdint.h>
#include "i2c.h"

#define MCP24LC32AT_DEV_ID    0x50

#define FIRST_NAME_ADDR				0x00
#define	LAST_NAME_ADDR				0x50
#define	TEAM_NUMBER_ADDR			0xA0

i2c_status_t eeprom_byte_write
( 
  uint32_t  i2c_base,
  uint16_t  eepromDataAddress,
  uint8_t   data
);

i2c_status_t eeprom_byte_read
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   *data
);
//*****************************************************************************
// Modifies the eeprom contents and then resets the board
//*****************************************************************************
void edit_eeprom(void);

#endif
