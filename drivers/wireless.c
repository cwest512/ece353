#include "wireless.h"

extern void spiTx(uint32_t base, uint8_t *tx_data, int size, uint8_t *rx_data);
extern bool spiVerifyBaseAddr(uint32_t base);

NORDIC_CONFIG wirelessPinConfig;

//*****************************************************************************
// Busy wait for roughly 15uS.
//*****************************************************************************
void wait_uS_15(void)
{
  int i = 750;
  while(i >0)
  {
    i--;
  }
}

//*****************************************************************************
// Configures the pins and peripherals used to send data to the wireless 
// driver.  The eliminates the need to hardcode a specific SPI peripheral 
// into the driver software.
//*****************************************************************************
void wireless_set_pin_config(
  uint32_t  wireless_spi_base, 
  uint8_t   payload_size,
  uint8_t   channel,
  uint32_t  csn_base,
  uint8_t   csn_pin,
  uint32_t  ce_base,
  uint8_t   ce_pin
)
{
  wirelessPinConfig.wireless_spi_base = wireless_spi_base;
  wirelessPinConfig.payload_size = payload_size;
  wirelessPinConfig.channel = channel;
  wirelessPinConfig.csn_base = csn_base;
  wirelessPinConfig.csn_pin = csn_pin;
  wirelessPinConfig.ce_base = ce_base;
  wirelessPinConfig.ce_pin = ce_pin;
}

 //*****************************************************************************
// Manually sets the SPI chip select line low
//*****************************************************************************
static __INLINE void  wireless_CSN_low(void)
{
  GPIOA_Type *CSN_BASE = (GPIOA_Type *)wirelessPinConfig.csn_base;
  CSN_BASE->DATA &= ~wirelessPinConfig.csn_pin;
}

//*****************************************************************************
// Manually sets the SPI chip select line high
//*****************************************************************************
static __INLINE void  wireless_CSN_high(void)
{
  GPIOA_Type *CSN_BASE = (GPIOA_Type *)wirelessPinConfig.csn_base;
  CSN_BASE->DATA |= wirelessPinConfig.csn_pin;
}

//*****************************************************************************
// Sets the GPIO pin connected to the CE pin on the nRF24L01+ low
//*****************************************************************************
static __INLINE void  wireless_CE_low(void)
{
  
  GPIOA_Type *CE_BASE = (GPIOA_Type *)wirelessPinConfig.ce_base;
  CE_BASE->DATA &= ~wirelessPinConfig.ce_pin;
}

//*****************************************************************************
// Sets the GPIO pin connected to the CE pin on the nRF24L01+ high
//*****************************************************************************
static __INLINE void  wireless_CE_high(void)
{
  
  GPIOA_Type *CE_BASE = (GPIOA_Type *)wirelessPinConfig.ce_base;
  CE_BASE->DATA |= wirelessPinConfig.ce_pin;
}

//*****************************************************************************
// Pulses the CE line high for roughly 15uS
//*****************************************************************************
static __INLINE void  wireless_CE_Pulse(void)
{
  wireless_CE_high();
  
  wait_uS_15();
  
  wireless_CE_low();
}

//*****************************************************************************
// ADD CODE
// This function reads a single byte of data from the register at the 5-bit 
// address specificed by the lower 5 bits of paramter reg. 
//
// The value at that register is returned to the  user as a uint8_t.  
//
// Page 46 of the data sheet lists the supported commnads for the nRF24L01+.  
// The first two entries entries describe how to read/write a single byte of 
// data to a register.
//
// Use spiTx() to send the data via the SPI interface.  This function can be
// found in spi.c.  You will also need to use wireless_CSN_low() and 
// wireless_CSN_high() to manually set the SPI chip select.
//
// Use wirelessPinConfig.wireless_spi_base to determine the SPI_BASE address
// sent to spiTx();
//*****************************************************************************
static __INLINE uint8_t wireless_reg_read(uint8_t reg)
{
	uint8_t command[NUM_BYTES];
	uint8_t received[NUM_BYTES];
	//Isolate last 5 bits
	reg &= 0x1F;
	command[1] = 0;
	command[0] = reg;
	wireless_CSN_low();
	spiTx(wirelessPinConfig.wireless_spi_base,command,NUM_BYTES,received);
	wireless_CSN_high();
	return received[1];
}

//*****************************************************************************
// ADD CODE
// This function writes a single byte of data from the register at the 5-bit 
// address specificed by the lower 5 bits of paramter reg. 
//  
// Page 46 of the data sheet lists the supported commnads for the nRF24L01+.  
// The first two entries entries describe how to read/write a single byte of 
// data to a register.
//
// Use spiTx() to send the data via the SPI interface.  This function can be
// found in spi.c.  You will also need to use wireless_CSN_low() and 
// wireless_CSN_high() to manually set the SPI chip select.
//*****************************************************************************
static __INLINE void wireless_reg_write(uint8_t reg, uint8_t data)
{
	uint8_t command[NUM_BYTES];
	uint8_t received[NUM_BYTES];
	//Isolate last 5 bits
	reg &= 0x1F;
	//Shift bits to MSB
	command[1] = data;
	command[0] = (1 << 5) | reg;
	wireless_CSN_low();
	spiTx(wirelessPinConfig.wireless_spi_base,command,NUM_BYTES,received);
	wireless_CSN_high();

}




//*****************************************************************************
// ADD CODE
// This function writes 5 bytes of data to the TX_ADDR register 
// Assume that tx_addr is an array of size 5.
// Send the data over starting with the byte at the lowest address in tx_addr.
//
// Use spiTx() to send the data via the SPI interface.  This function can be
// found in spi.c.  You will also need to use wireless_CSN_low() and 
// wireless_CSN_high() to manually set the SPI chip select.
//*****************************************************************************
static __INLINE uint32_t wireless_set_tx_addr(uint8_t  *tx_addr)
{
	int i;
	uint8_t command[6];
	uint8_t received[6];
	command[0] = (1 << 5) | NRF24L01_TX_ADDR_R;
	
	for(i = 1; i < 6; i++)
	{
		command[i] = *tx_addr;
		tx_addr++;
	}
	wireless_CSN_low();
	spiTx(wirelessPinConfig.wireless_spi_base,command,6,received);
	wireless_CSN_high();
	return (uint32_t) *received;
}

//*****************************************************************************
// ADD CODE
// This function writes 4 bytes of data to the nRF24L01+ Tx FIFO using the 
// W_TX_PAYLOAD command found on page 46 of the nRF24L01+ datasheet.  Send
// the most significant byte first.
//
// Use spiTx() to send the data via the SPI interface.  This function can be
// found in spi.c.  You will also need to use wireless_CSN_low() and 
// wireless_CSN_high() to manually set the SPI chip select.
//*****************************************************************************
static __INLINE void wireless_tx_data_payload( uint32_t data)
{
	int i;
	uint8_t command[5];
	uint8_t received[5];
	uint8_t* data_arr = (uint8_t*) &data;
	command[0] = 0xA0;
	command[1] = data_arr[3];
	command[2] = data_arr[2];
	command[3] = data_arr[1];
	command[4] = data_arr[0];
	
	//printf("Data_arr: %d %d %d %d %d\n",command[0],command[1],command[2],command[3],command[4]);
	wireless_CSN_low();
	spiTx(wirelessPinConfig.wireless_spi_base,command,5,received);
	wireless_CSN_high();
}



//*****************************************************************************
// ADD CODE
// This function reads 4 bytes of data from the nRF24L01+ Tx FIFO using the 
// R_RX_PAYLOAD command found on page 46 of the nRF24L01+ datasheet.  Data will
// be received most significant byte first.
//
// Use spiTx() to send the data via the SPI interface.  This function can be
// found in spi.c.  You will also need to use wireless_CSN_low() and 
// wireless_CSN_high() to manually set the SPI chip select.
//*****************************************************************************
static __INLINE void wireless_rx_data_payload( uint32_t *data)
{
	uint8_t* ptr;
	uint8_t rx_data[5];
	uint8_t tx_data[5] = {0,0,0,0,0};
	tx_data[0] = NRF24L01_CMD_R_RX_PAYLOAD;
	wireless_CSN_low();	
	spiTx(wirelessPinConfig.wireless_spi_base,tx_data,5,rx_data);
	ptr = (uint8_t*) data;
	ptr[3] = rx_data[1];
	ptr[2] = rx_data[2];
	ptr[1] = rx_data[3];
	ptr[0] = rx_data[4];
	wireless_CSN_high();
}

//****************************************************************************
// Sets the rx address of a given RX pipe.  See nRF24L01+ for more details
//*****************************************************************************
static __INLINE int32_t wireless_set_rx_addr(
  uint8_t  *rx_addr,
  uint8_t   pipe
)
{
  uint8_t tx_data[6];
  uint8_t rx_data[6];
  uint8_t wireless_reg = 0;
  
  switch(pipe)
  {
    case 0: { wireless_reg = NRF24L01_RX_ADDR_P0_R; break; }
    case 1: { wireless_reg = NRF24L01_RX_ADDR_P1_R; break; }
    case 2: { wireless_reg = NRF24L01_RX_ADDR_P2_R; break; }
    case 3: { wireless_reg = NRF24L01_RX_ADDR_P3_R; break; }
    case 4: { wireless_reg = NRF24L01_RX_ADDR_P4_R; break; }
    case 5: { wireless_reg = NRF24L01_RX_ADDR_P5_R; break; }
    default: {return -1;}
  }

  tx_data[0] = wireless_reg |  NRF24L01_CMD_W_REGISTER   ;
  tx_data[1] = *(rx_addr +0 );
  tx_data[2] = *(rx_addr +1 );
  tx_data[3] = *(rx_addr +2 );
  tx_data[4] = *(rx_addr +3 );
  tx_data[5] = *(rx_addr +4 );
  wireless_CSN_low();
  spiTx(wirelessPinConfig.wireless_spi_base,tx_data, 6, rx_data);
  wireless_CSN_high();
  return 0;
}


//*****************************************************************************
// Clears any trasmitted messages in the TX FIFO.  Messages that fail to 
// transmit fill up the FIFO, so we need to remove them.
//*****************************************************************************
static __INLINE void wireless_flush_tx_fifo( void)
{
  uint8_t tx_data[1];
  uint8_t rx_data[1];
  
    tx_data[0] = NRF24L01_CMD_FLUSH_TX;
    wireless_CSN_low();
    spiTx(wirelessPinConfig.wireless_spi_base,tx_data, 1, rx_data);
    wireless_CSN_high();
}


//*****************************************************************************
// On initialization, we remove any messages that are sitting in the RX Fifo
//*****************************************************************************
static __INLINE void wireless_flush_rx_fifo( void )
{
  uint8_t tx_data[1];
  uint8_t rx_data[1];
  
    tx_data[0] = NRF24L01_CMD_FLUSH_RX;
    wireless_CSN_low();
    spiTx(wirelessPinConfig.wireless_spi_base,tx_data, 1, rx_data);
    wireless_CSN_high();
}


//*****************************************************************************
// Places the nRF24L01+ into transmit mode
//*****************************************************************************
static __INLINE void wireless_start_tx_mode( void )
{
  uint8_t config_reg;
  
  // Read in the current config
  config_reg = wireless_reg_read( NRF24L01_CONFIG_R);
  
  // Clear PRIM_RX
  wireless_reg_write(NRF24L01_CONFIG_R, config_reg & ~NRF24L01_CONFIG_PRIM_RX_PRX);  
}

//*****************************************************************************
// Places the nRF24L01+ into receive mode
//*****************************************************************************
static __INLINE void wireless_start_rx_mode( void)
{
  uint8_t config_reg;
  
  // Read in the current config
  config_reg = wireless_reg_read( NRF24L01_CONFIG_R);
  
  // Clear PRIM_RX
  wireless_reg_write(NRF24L01_CONFIG_R, config_reg | NRF24L01_CONFIG_PRIM_RX_PRX); 
}

//*****************************************************************************
// Checks to see if there is the Rx FIFO is empty
//*****************************************************************************
static __INLINE bool wireless_rx_fifo_empty (void )
{
  uint8_t status = wireless_reg_read(NRF24L01_FIFO_STATUS_R);
  if( (status & NRF24L01_FIFO_STATUS_RX_EMPTY_M ) != 0)
  {
    return true;
  }
  else
  {
     return false;
  }
}



//*****************************************************************************
// Examines an 8-bit status bit to determine if the RX_DR bit is set
// (Data Received)
//*****************************************************************************
static __INLINE bool wireless_status_rx_dr_asserted(uint8_t status)
{
  if ( (status & NRF24L01_STATUS_RX_DR_M) != 0)
    return true;
  else
    return false;
}

//*****************************************************************************
// Examines an 8-bit status bit to determine if the TX_DS bit is set
// (Data Sent)
//*****************************************************************************
static __INLINE bool wireless_status_tx_ds_asserted(uint8_t status)
{
  if ( (status & NRF24L01_STATUS_TX_DS_M) != 0)
    return true;
  else
    return false;
}

//*****************************************************************************
// Examines an 8-bit status bit to determine if the MAX_RT bit is set
// (Max Retries)
//*****************************************************************************
static __INLINE bool wireless_status_max_rt_asserted(uint8_t status)
{
  if ( (status & NRF24L01_STATUS_MAX_RT_M) != 0)
    return true;
  else
    return false;
}

//*****************************************************************************
// Examines an 8-bit status bit to determine if the TX FIFO full bit is set
//*****************************************************************************
static __INLINE bool wireless_status_tx_full_asserted(uint8_t status)
{
  if ( (status & NRF24L01_STATUS_TX_FULL_M) != 0)
    return true;
  else
    return false;
}

//*****************************************************************************
// Returns the contents of the nRF24L01+ status register
//*****************************************************************************
static __INLINE uint8_t wireless_get_status( void )
{
  uint8_t tx_data[1];
  uint8_t rx_data[1];
  
  tx_data[0] = NRF24L01_CMD_NOP;
  wireless_CSN_low();
  spiTx(wirelessPinConfig.wireless_spi_base,tx_data, 1, rx_data);
  wireless_CSN_high();
  
  return rx_data[0];
}


//*****************************************************************************
// Clears the retry exceeded status bit in the status register.
// When a packet is dropped, we must clear this before we can transmit any
// further data.
//*****************************************************************************
static __INLINE void wireless_clear_max_rt( void )
{
  wireless_reg_write( NRF24L01_STATUS_R, NRF24L01_STATUS_MAX_RT_M);
}

//*****************************************************************************
// Clears the data sent status bit in the status register.
// When a packet is dropped, we must clear this before we can transmit any
// further data.
//*****************************************************************************
static __INLINE void wireless_clear_tx_ds( void )
{
  wireless_reg_write( NRF24L01_STATUS_R, NRF24L01_STATUS_TX_DS_M);
}

//*****************************************************************************
// Waits until the data has been successfully transmitted.  
//
// If the data is successfully sent, true is returned.
//
// If the data packet is dropped and the re-try count has been exeeded, 
// then false is returned.
//*****************************************************************************
static __INLINE bool wireless_wait_for_tx_ds( void )
{
  uint8_t status =  wireless_get_status() ;
  
  while( (wireless_status_tx_ds_asserted(status)==false) && (wireless_status_max_rt_asserted(status))==false)
   {
      status =  wireless_get_status() ;
   }
    
    // Indicate if the data was sucessfully sent
    if(wireless_status_tx_ds_asserted(status))
    {
      return true;
    }
    else
    {
      return false;
    }
    
}

//*****************************************************************************
// Waits until new data has arrived in the Rx FIFO
//*****************************************************************************
static  __INLINE void wireless_wait_for_rx_dr( void )
{
   uint8_t status = wireless_get_status() ;
  
   while( wireless_status_rx_dr_asserted(status) == false)
   {
      status = wireless_get_status() ;
   }
   
    // Clear the RX_DR bit
    wireless_reg_write( NRF24L01_STATUS_R, NRF24L01_STATUS_RX_DR_M); 
   
}



//*****************************************************************************
// Public Functions
//*****************************************************************************

//*****************************************************************************
// Transmits 4 bytes of data to the remote device.
//
// A return code of NRF24L01_TX_SUCCESS is used to indicate that the data
// was transmitted sucessfully
//*****************************************************************************
wireless_com_status_t 
wireless_send_32(
  bool      blockOnFull,
  bool      retry,
  uint32_t  data
)
{ 
  uint8_t status;
  
  if( spiVerifyBaseAddr(wirelessPinConfig.wireless_spi_base))
  {
    // Check the status of the device
    status = wireless_get_status();
    
    if( wireless_status_tx_full_asserted(status) && (blockOnFull == false))
    {
       return NRF24L01_TX_FIFO_FULL;
    }
    
    // Wait while the TX FIFO is not full 
    while(wireless_status_tx_full_asserted(status))
    {
        status = wireless_get_status();
    }
    
    do
    {
      // Put into Standby-1
      wireless_CE_low();
      wireless_clear_max_rt();
      
      // Set tx_mode
      wireless_start_tx_mode();
      
      // Flush any outstanding info in the TX FIFO
      wireless_flush_tx_fifo();
      
      // Send the data to the TX_PLD
      wireless_tx_data_payload(data);
      
      // Pulse CE for a 15uS
      wireless_CE_Pulse();
      
       status = wireless_wait_for_tx_ds();
      
       if( status == false)
       {
         //printf(" ** ERR ** MAX_RT exceeded\n\r");
         wireless_clear_max_rt();

       }
       else
       {
          // Clear the tx_ds bit
          wireless_clear_tx_ds();
       }
    } while( status == false && retry == true);
    

     // Set PRIM_RX High
      wireless_start_rx_mode();
      
      // Enable Wireless transmission
      wireless_CE_high();
    
    if (status == true)
    {
      return NRF24L01_TX_SUCCESS;
    }
    else
    {
      return NRF24L01_TX_PCK_LOST;
    }
  }
  else
  {
    return NRF24L01_ERR;
  }
  
}

//*****************************************************************************
// Receives 4 bytes of data from the remote board.  The user can optionally
// block until data arrives.
//
// The data received is returned in the *data paramter
// 
// Returns NRF24L01_RX_SUCCESS on a successful packet reception
//*****************************************************************************
wireless_com_status_t
wireless_get_32(
  bool      blockOnEmpty,
  uint32_t  *data
)
{
  //uint8_t status;
  if( spiVerifyBaseAddr(wirelessPinConfig.wireless_spi_base))
  {

    if( wireless_rx_fifo_empty() == false)
    {
      // Read data from Rx FIFO
      wireless_rx_data_payload(data);
      
      // If Rx FIFO is empty, clear the RX_DR bit in the status
      // register
      if ( wireless_rx_fifo_empty() ==  true)
      {
         // Clear the RX_DR bit
          wireless_reg_write(NRF24L01_STATUS_R, NRF24L01_STATUS_RX_DR_M);
      }
    
      return NRF24L01_RX_SUCCESS;
    }
    else if ( (wireless_rx_fifo_empty() == true) && blockOnEmpty)
    {
      
     // Wait until the RX_DR bit is set
      wireless_wait_for_rx_dr();
      
      // Read data from Rx FIFO
      wireless_rx_data_payload( data);
      
      // If Rx FIFO is empty, clear the RX_DR bit in the status
      // register
      if ( wireless_rx_fifo_empty() ==  true)
      {
         // Clear the RX_DR bit
          wireless_reg_write(NRF24L01_STATUS_R, NRF24L01_STATUS_RX_DR_M);
      }
      
      return NRF24L01_RX_SUCCESS;
    }
    else
    {
      return NRF24L01_RX_FIFO_EMPTY;
    }
  }
  else
  {
    return NRF24L01_ERR;
  }
}

//*****************************************************************************
// Configures the Nordic nRF24L01+ to be in a point-to-point configuration.
// 
// my_id is a pointer to a 5-byte array that contains the id of the local
// nordic radio
//
// dest_id is a pointer to the 5-byte array that contains the id of the remote
// nordic radio.
//*****************************************************************************
bool wireless_configure_device( 
  uint8_t           *my_id,
  uint8_t           *dest_id
)
{
  
  if( spiVerifyBaseAddr(wirelessPinConfig.wireless_spi_base))
  {
    wireless_CSN_high();
    wireless_CE_low();
    
    // Configure Common RF settings
    wireless_flush_tx_fifo();
    wireless_flush_rx_fifo();
    wireless_reg_write(NRF24L01_RF_SETUP_R, NRF24L01_RF_SETUP_RF_PWR_0DB | NRF24L01_RF_SETUP_250_KBPS);
    wireless_reg_write(NRF24L01_RF_CH_R, wirelessPinConfig.channel);
    wireless_reg_write( NRF24L01_STATUS_R, NRF24L01_STATUS_CLEAR_ALL);
    wireless_reg_write(NRF24L01_SETUP_RETR_R, NRF24L01_SETUP_RETR_ARD_0750_US | NRF24L01_SETUP_RETR_ARC_15);
    
    // Configure the address to transfer data to
    wireless_set_tx_addr(dest_id);
    
    // Configure Pipe 0 to receive the AUTO ACKs from the other device
    wireless_reg_write(NRF24L01_RX_PW_P0_R, wirelessPinConfig.payload_size);
    wireless_set_rx_addr(dest_id, 0);
      
    // Configure Pipe 1
    wireless_reg_write(NRF24L01_RX_PW_P1_R, wirelessPinConfig.payload_size);
    wireless_set_rx_addr(my_id, 1);
    
    // Turn on Rx and AutoAcks for pipe 0 and 1
    wireless_reg_write(NRF24L01_EN_RXADDR_R, NRF24L01_RXADDR_ERX_P0 | NRF24L01_RXADDR_ERX_P1); 
    wireless_reg_write(NRF24L01_EN_AA_R, NRF24L01_ENAA_P0 | NRF24L01_ENAA_P1);

    // Enable the Radio in RX mode
    wireless_reg_write(NRF24L01_CONFIG_R,NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_PRIM_RX_PRX | 0x20 | 0x10 );
      
    wireless_CE_high();
    return true;
  }
  else
  {
    return false ;
  }
  
} 


