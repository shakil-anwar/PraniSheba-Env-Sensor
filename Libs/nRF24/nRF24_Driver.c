/*
   NRF24_Driver.c

    Created on: Aug 27, 2020
        Author: Shuvangkar
*/
#include "nRF24_Driver.h"


volatile uint8_t nrf_status;
volatile bool _nrfDebug;
/*********GPIO variable***********/
 volatile uint8_t *_cePort;
 volatile uint8_t _cePin;
 volatile uint8_t *_csnPort;
 volatile uint8_t _csnPin;

/***********************Driver Main functions************************/
uint8_t read_register(uint8_t addr)
{
  uint16_t response;

  nrf_csn_low();
  response = spi_transfer16(RF24_NOP | ((addr & RF24_REGISTER_MASK) << 8));
  nrf_csn_high();

  nrf_status = (uint8_t) ((response & 0xFF00) >> 8);
  return (uint8_t) (response & 0x00FF);
}

void write_register(uint8_t addr, uint8_t data)
{
  uint16_t response;

  nrf_csn_low();
  response = spi_transfer16( (data & 0x00FF) | (((addr & RF24_REGISTER_MASK) | RF24_W_REGISTER) << 8) );
  nrf_csn_high();

  nrf_status = (uint8_t) ((response & 0xFF00) >> 8);
}

void write_bytes_in_register(uint8_t addr, uint8_t *payload, uint8_t len)
{
  nrf_ce_low();
  uint8_t *ptr = payload;

  nrf_csn_low();
  nrf_status = spi_transfer(addr | RF24_W_REGISTER);
  for (uint8_t i = 0; i < len ; i++)
  {
    spi_transfer(ptr[i]);
  }
  nrf_csn_high();
}

uint8_t *read_bytes_in_register(uint8_t addr, uint8_t *bucket, uint8_t len)
{
  uint8_t *ptr = bucket;

  nrf_csn_low();
  nrf_status = spi_transfer(addr & RF24_REGISTER_MASK);
  for (uint8_t i = 0; i < len; i++)
  {
    ptr[i] = spi_transfer(0xFF);
  }
  nrf_csn_high();

  return bucket;
}

void set_reg_bit(uint8_t reg, uint8_t bitMask)
{
	uint8_t regValue = read_register(reg);
	write_register(reg,regValue | bitMask);
}

void clear_reg_bit(uint8_t reg, uint8_t bitMask)
{
	uint8_t regValue = read_register(reg);
	write_register(reg, regValue & ~bitMask);
}

void nrf_flush_tx()
{
  nrf_csn_low();
  nrf_status = spi_transfer(RF24_FLUSH_TX);
  nrf_csn_high();

}
void nrf_flush_rx()
{
  nrf_csn_low();
  nrf_status = spi_transfer(RF24_FLUSH_RX);
  nrf_csn_high();
}

/****************************Driver Main functions ended*********************/

void nrf_set_tx_addr(uint8_t *addr, uint8_t len)
{
  write_bytes_in_register(RF24_TX_ADDR, addr, len);
}

void nrf_set_rx_addr(uint8_t pipe, uint8_t *addr, uint8_t len)
{

  if (pipe > 5)
  {
    pipe = 1;
  }

  if (pipe > 1)
  {
    write_register(RF24_RX_ADDR_P0 + pipe, addr[0]);
  } 
  else 
  {
    write_bytes_in_register(RF24_RX_ADDR_P0 + pipe, addr, len);
  }

  //RX Address PIPE Enable
  set_reg_bit(RF24_EN_RXADDR, (1 << pipe));
}

void nrf_set_addr_width(uint8_t width)
{
  if (width < 3 || width > 5)
  {
    return;
  }
  write_register(RF24_SETUP_AW, (width - 2) & 0b00000011);
}


void nrf_set_tx_dbm_speed(uint8_t power_speed)
{
  write_register(RF24_RF_SETUP, (power_speed) & 0x2F);
}

void dynpd_disable(uint8_t pipe)
{
	clear_reg_bit(RF24_DYNPD,1<<pipe);

}

void dynpd_enable(uint8_t pipe)
{
	set_reg_bit(RF24_DYNPD,1<<pipe);
}

void nrf_debug_register(uint8_t addr)
{
  uint8_t reg = read_register(addr);
  //serial_debug_print_uint8(reg, 2);
}




















// void nrf_enable_ack(uint8_t pipe)
// {
//   //Enable Dynamic payload feature
//   write_register(RF24_FEATURE, EN_DLP); 
  
//   //Enable Dynamic paylod on pipe
//   uint8_t readReg = read_register(RF24_DYNPD);
//   write_register(RF24_DYNPD, 0x3F);
  
//   //Enable Auto acknowledgment on pipe
//   readReg = read_register(RF24_EN_AA);
//   write_register(RF24_EN_AA, readReg | (1 << pipe));
// }

// void nrf_write_tx_payload(uint8_t *data, uint8_t len)
// {
//   nrf_flush_tx(); //flushing the tx buffer before new data
//   write_bytes_in_register(RF24_W_TX_PAYLOAD, data, len);
// }

// uint8_t *nrf_read_rx_payload(uint8_t *data, uint8_t len)
// {
//   uint8_t *ptr = data;
//   nrf_csn_low();
//   spi_transfer(RF24_R_RX_PAYLOAD);
//   uint8_t i;
//   for(i = 0;i<len; i++)
//   {
//     ptr[i] = spi_transfer(RF24_NOP);
//   }
//   nrf_csn_high();
//   // nrf_flush_rx();
//   return data;
// }


// void nrf_start_transmit()
// {
//   nrf_ce_high();
//   delay_us(15);
//   nrf_ce_low();
//   nrf_ce_high();
// #if defined(ARDUINO_ARCH_AVR)
//   delayMicroseconds(15);
// #else
//   __delay_cycles(15);
// #endif
//   nrf_ce_low(); 

// }

// void nrf_start_receive()
// {
//   nrf_ce_high();
// }

// void nrf_clr_tx_int()
// {

//   write_register(RF24_STATUS, TX_DS | MAX_RT);

// }
// void nrf_clr_rx_int()
// {
//   write_register(RF24_STATUS, RX_DR);
// }

// void nrf_power_up_tx()
// {
//   write_register(RF24_CONFIG,
//                  EN_ALL_INT       |
//                  CRC_EN_2_BYTE    |
//                  PWR_UP           |
//                  TX_MODE);
// }

// void nrf_power_up_rx()
// {
//   write_register(RF24_CONFIG,
//                  EN_ALL_INT       |
//                  CRC_EN_2_BYTE    |
//                  PWR_UP           |
//                  RX_MODE);
// }

// void nrf_power_down()
// {
//   write_register(RF24_CONFIG,
//                  DIS_ALL_INT      |
//                  CRC_EN_2_BYTE    |
//                  PWR_DOWN         |
//                  TX_MODE);
// }

// bool is_tx_not_empty()
// {
//   return true;
// }

// void dynPayload(uint8_t pipe, bool onOff)
// {

//   uint8_t reg = read_register(RF24_FEATURE);
//   write_register(RF24_FEATURE, onOff<<EN_DLP_BIT); //Enable Dynamic payload

//   uint8_t reg = read_register(RF24_DYNPD);
//   write_register(RF24_DYNPD, dynpd | (1 << pipe));
// }


/****************************************New Implemented function*****************************/



