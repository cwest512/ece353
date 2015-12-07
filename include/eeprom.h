#ifndef __EEPROM_H__
#define __EEPROM_H__


#include <stdint.h>
#include "i2c.h"

#define MCP24LC32AT_DEV_ID    0x50

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


#endif
