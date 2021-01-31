/*
   nrf24.c

    Created on: Aug 31, 2020
        Author: sshuv
*/

#include "nRF24.h"

nrf_irq_state_t _nrfIrqState;

isrPtr_t tx_isr;
isrPtr_t rx_isr;
isrPtr_t max_rt_isr;

uint32_t (*_Millis)(void) = NULL;


bool isTXActive = false; //flag true when master sends query in tx mode


void nrfSetMillis(uint32_t (*ms)())
{
  _Millis = ms;
}

void nrfSetPin(uint8_t *cePort, uint8_t cePin, uint8_t *csnPort, uint8_t csnPin)
{
  _cePort = cePort;
  _cePin = cePin;
  _csnPort = csnPort;
  _csnPin = csnPin;

  #if defined(ARDUINO_ARCH_AVR)
  cePort--;
  csnPort--;
  #elif defined(__msp430)
  cePort++;
  csPort++;
  #endif

  *cePort |= 1<<cePin;// setting ddr register
  *csnPort |= 1<<csnPin; //setting ddr register
   
   *_cePort &= ~(1 << _cePin); //radio disable
   *_csnPort |=  (1<< _csnPin); //chip disable 
   
}

void nrfBegin(air_speed_t speed, power_t power, uint32_t spiSpeed)
{
  
  _nrfIrqState = NRF_WAIT;
  tx_isr = NULL;
  rx_isr = NULL;
  max_rt_isr = NULL;

  _nrfDebug = true;
  
  // initiate SPI
  spi_begin(spiSpeed);
  //initiate Serial
  //serial_begin();
  if(_nrfDebug){SerialBegin(250000);}
  delay(100); //Power on reset 100ms
// send NRF_NOP command
  nrf_csn_low();
  spi_transfer(RF24_NOP);
  nrf_csn_high();
  // Disable nrf Interrupts, set CRC 2 Bytes, POWER DOWN, TX MODE
  write_register(RF24_CONFIG,
                     DIS_ALL_INT |
                     CRC_EN_2_BYTE |
                     PWR_DOWN |
                     TX_MODE);

  // clear Interrupt flag
  write_register(RF24_STATUS,
                      TX_DS |
                      MAX_RT |
                      RX_DR);
  // disable all pipes
  write_register(RF24_EN_RXADDR, 0x00);
  // disable auto acknowledgement in all pipes
  write_register(RF24_EN_AA, 0x00);
  // disable dynamic payload in all pipes
  write_register(RF24_DYNPD, 0x00);
  // set default channel
  nrfSetCh(RF_CHANNALE_NO);
  // set payload len
  nrf_set_addr_width(DEFAULT_ADDR_LEN);
  // set retransmit count and retransmit delay
  write_register(RF24_SETUP_RETR, RETRANSMIT_OPCODE | RETRANSMIT_COUNT);
  // set transmit power and data speed
  nrf_set_tx_dbm_speed(((uint8_t)speed | (uint8_t)power));
  // flush rx
  nrf_flush_tx();
  // flush tx
  nrf_flush_rx();
  // enable interrupt
  write_register(RF24_CONFIG,
                  EN_ALL_INT    |
                  CRC_EN_2_BYTE |
                  PWR_DOWN      |
                  TX_MODE);
  
  if(_Millis == NULL)
  {
    SerialPrintF(P("_Millis is null, need time functions"));
  }
  SerialPrintlnF(P("NRF Setup done"));
}

/***********************************************************************************
 *                                         RX Functions
 * *********************************************************************************/    
void nrfSetPipe(uint8_t pipe, uint8_t *addr, bool ackFlag)
{
   nrf_ce_low(); // if device is power up, this ensures standby-1
  // set pipe addr
  nrf_set_rx_addr(pipe, addr, DEFAULT_ADDR_LEN);

  if (ackFlag == 1)
  {
    nrfEnableAck(pipe);
  }
}

void nrfEnableAck(uint8_t pipe)
{
  // enable dynamic payload length in feature
  dlp_enable();
  // enable dynamic payload on pipe
  dynpd_enable(pipe);
  //Enable Auto acknowledgment on pipe
  set_reg_bit(RF24_EN_AA, (1 << pipe));
}

void nrfRxStart()
{
  nrf_ce_low();
  nrfClearIrq();
  nrf_flush_rx();
  nrfRXMode();
  nrf_ce_high();
  delay_us(150);
}

/***********************************************************************************
 *                                         TX Functions
 * *********************************************************************************/

void nrfSetTx(uint8_t *addr, bool ackFlag)
{
  nrf_ce_low();// if device is power up, this ensures standby-1
  // set TX address
  write_bytes_in_register(RF24_TX_ADDR, addr, DEFAULT_ADDR_LEN);
  
  if (ackFlag == 1)
  {
  // set address in pipe 0
    nrf_set_rx_addr(0,addr,DEFAULT_ADDR_LEN);
  // enable auto ack on pipe 0
    nrfEnableAck(0);
  }
}

void nrfTXStart()
{
  nrf_ce_low();
  nrfClearIrq();
  nrf_flush_tx();
  nrfTXMode();
  nrf_ce_low(); //NRF goes to Standby -1. 
  // delay_us(130);
}


void nrfStartTransmit()
{
  nrf_ce_high(); 
  delayMicroseconds(15);
  nrf_ce_low();
}



/***********************************************************************************
 *                                         NRF Status Functions
 * *********************************************************************************/
bool nrfIsRunning()
{
  return ((read_register(RF24_CONFIG) & CRC_EN_2_BYTE)>>2);
}

bool nrfRxFifiFull()
{
  // check RXFULL pin RF24_FIFO_STATUS register
  if(read_register(RF24_FIFO_STATUS) & RX_FULL) return true;
  else return false;
  
}
uint8_t nrfReadStatus()
{
  // send RF24_NOP return output
  nrf_csn_low();
  uint8_t reg = spi_transfer(RF24_NOP);
  nrf_csn_high();
  return reg;
}

uint8_t nrfPayloadLen()
{
  uint16_t response;
  nrf_csn_low();
  response = spi_transfer16((RF24_R_RX_PL_WID << 8) | RF24_NOP);
  nrf_csn_high();
  return (uint8_t)(response);
}

bool nrfIntStatus(uint8_t bitPos)
{
  nrf_csn_low();
  uint8_t reg = spi_transfer(RF24_NOP);
  nrf_csn_high();
  return ((reg >>bitPos) & 0x01);
}


uint8_t pipeAvailable()
{
  // check pipe no in RF24_STATUS register
  nrf_csn_low();
  uint8_t reg = spi_transfer(RF24_NOP);
  nrf_csn_high();
  return ((reg>>1) & PIPE_MASK);
}

uint8_t pipeAvailFast()
{
  //This functions return pipe availableif nrf_status is done reading already
  return ((nrf_status>>1) & PIPE_MASK);
}

bool nrfIsRxEmpty()
{
  //returns true if RX FIFO empty
  // uint8_t reg = read_register(RF24_FIFO_STATUS);
  return (read_register(RF24_FIFO_STATUS) & RX_EMPTY);
}

bool nrfTxIsFifoEmpty()
{
  return (read_register(RF24_FIFO_STATUS) & TX_EMPTY);
}

nrf_mode_t nrfWhichMode()
{
  uint8_t config = read_register(RF24_CONFIG);
  if(config & PWR_UP)
  {
    //Chip power up
    uint8_t cePin = nrf_ce_read();
    if(_nrfDebug){SerialPrintF(P("CE_PIN : "));SerialPrintlnU8(cePin && true);}
    if(config & RX_MODE)
    {
      if(cePin)
      {
        if(_nrfDebug){SerialPrintlnF(P("<RXMODE>"));}
        return RXMODE;
      }
    }
    else
    {
      if(cePin)
      {
        if(nrfTxIsFifoEmpty())
        {
          if(_nrfDebug){SerialPrintlnF(P("<STANDBY2>"));}
          return STANDBY2;
        }
        else
        {
          if(_nrfDebug){SerialPrintlnF(P("<TXMODE>"));}
          return TXMODE;
        }
        
      }
      else
      {
        if(_nrfDebug){SerialPrintlnF(P("<STANDBY1>"));}
        return STANDBY1;
      }
      
    }
  }
  else
  {
    //chip power down
    if(_nrfDebug){SerialPrintlnF(P("<POWER_DOWN>"));}
    return POWER_DOWN;
  }
  
}

/***********************************************************************************
 *                                         NRF Basic Read/Write Functions
 * *********************************************************************************/

void nrfWrite(const uint8_t *data, uint8_t len)
{
  nrf_ce_low();//Goes to Standby-1 before writing payload.
  // write_bytes_in_register(RF24_W_TX_PAYLOAD, data, len);
  nrf_flush_tx();
  nrf_csn_low();
  spi_transfer(RF24_W_TX_PAYLOAD);
  uint8_t i;
  for(i = 0; i<len; i++)
  {
    spi_transfer(data[i]);
  }
  nrf_csn_high();
  _nrfIrqState = NRF_WAIT;

}

void nrfSend(const uint8_t *data, uint8_t len)
{
  nrfWrite(data,len);
  nrfStartTransmit();
}

bool nrfAck()
{
  uint32_t currentMs = _Millis();
  uint32_t prevMs = currentMs;
  do
  {
    if(_nrfIrqState == NRF_SUCCESS)
    {
      return true;
    }
    else if(_nrfIrqState == NRF_FAIL)
    {
      break;
    }
    currentMs = _Millis();
  }while((currentMs - prevMs) < NRF_TRANSMIT_TIMEOUT);
  return false; 
}

bool nrfAckSend(const uint8_t *data, uint8_t len)
{
  nrfWrite(data,len);
  nrfStartTransmit();
  return nrfAck();
}


uint8_t *nrfRead(uint8_t *buffer, uint8_t len)
{
  // nrf_read_rx_payload(buffer,len);
  // read RX_received buffer
  // nrf_read_rx_payload(buffer,len);

  uint8_t *ptr = buffer;
  nrf_csn_low();
  spi_transfer(RF24_R_RX_PAYLOAD);
  uint8_t i;
  for(i = 0; i<len; i++)
  {
    ptr[i] = spi_transfer(RF24_NOP);
  }
  nrf_csn_high();
  nrf_flush_rx();
  return buffer;
}

void nrfWriteAck(uint8_t *data, uint8_t len)
{
  // write ack payload to buffer
}

void nrfReUseTX()
 {
     nrf_csn_low();
     nrf_status = spi_transfer(RF24_REUSE_TX_PL);
     nrf_csn_high();
     nrfStartTransmit();
 }


/***********************************************************************************
 *                                         NRF IRQs
 * *********************************************************************************/

void nrfSetIrqs(isrPtr_t txIsr, isrPtr_t rxIsr, isrPtr_t maxRtIsr)
{
  tx_isr = txIsr;
  rx_isr = rxIsr;
  max_rt_isr = maxRtIsr;
}

void nrfIrq()
{
  nrfClearIrq(); //This function clear IRQ bits and read the status in nrf_status
  // nrf_status = nrfReadStatus();
  if(_nrfDebug){SerialPrintF(P("NRF STATUS:"));SerialPrintlnU8(nrf_status);}
  if(nrf_status & TX_DS)
  {
    if(tx_isr)
    {
      tx_isr();
    }
    _nrfIrqState = NRF_SUCCESS;
    if(_nrfDebug){SerialPrintlnF(P("<tx_irq>"));}
  }
  else if(nrf_status & RX_DR)
  {
    if(rx_isr)
    {
      rx_isr();
    }
    _nrfIrqState = NRF_RECEIVED;
    if(_nrfDebug){SerialPrintlnF(P("<rx_irq>"));}
  }
  else
  {
    if(max_rt_isr)
    {
      max_rt_isr();
    }
    _nrfIrqState = NRF_FAIL;
    if(_nrfDebug){SerialPrintlnF(P("<max_rt_irq>"));}
  }
}


nrf_irq_state_t waitAck()
{
  return _nrfIrqState;
}

/***********************************************************************************
 *                                         NRF Utility
 * *********************************************************************************/
void nrfSetPldAck()
{
  // set acknowledgment payload ack FEATURE register
  set_reg_bit(RF24_FEATURE,EN_ACK_PAY);
}

void nrfClearIrq()
{
  // clear all irq in flag = 0
  // or clear specific flag in status register
  write_register(RF24_STATUS, MAX_RT | TX_DS | RX_DR);
}

void nrfRestorToRxTx()
// void nrfEnable()
{
  //This function will be called during operation
  //So after setting nrf_ce_high(), NRF will go to TX mode or RX mode
  //depending on the value of PRIM_RX bit
  nrf_ce_high();
}

void nrfRxTxToStandy1()
{
  //This function will be called during operation
  //So after setting nrf_ce_low(), NRF will go to Standby-1 whether it
  // is tx or rx mode
  nrf_ce_low();
}

void nrfSetCh(uint8_t ch)
{
  // set nrf channel
  //If channel exceeds rset value to 100 channel.
  if (ch > 125)
  {
    ch = 100;
  }
  write_register(RF24_RF_CH, (ch & 0b01111111));  
}


void nrfForceRxMode()
{
  if(nrfWhichMode() != RXMODE)
  {
    //change to rx mode
    if(isTXActive == false)
    {
      if(_nrfDebug){SerialPrintlnF(P("Changed to RX_MODE"));}
      nrfRxStart();
    }
  }
}

/***********************************************************************************
 *                                         NRF Extended Feature
 * *********************************************************************************/

void nrfWrite96(uint8_t *data, uint8_t len)
{
  uint8_t count = 0, success = 0;
  // write 96 byte to RX FIFO Buffer
  //flush TX buffer to write new data
  nrf_flush_tx();
  // write payload to TX
  write_bytes_in_register(RF24_W_TX_PAYLOAD, data, len);
  // check TxFifoFull or not
  if(read_register(RF24_FIFO_STATUS) & TX_FULL)
  {
    nrf_ce_high();
    while(1)
    {
      // if(waitAck() == NRF_IDLE)
      // {
      //   count++;
      //   if(_nrfIrqState == NRF_SUCCESS)
      //   {
      //     success++;
      //   }

      //   if(count>=3){
      //     nrf_ce_low();
      //     break;
      //   }
      // }
    }
  }
  else
  {
    // Serial.print("data pipe is not full")
  }
}


/***********************************************************************************
 *                                         NRF Debug Print
 * *********************************************************************************/
 void nrfDebugPrint()
 {
    //serial_print("\r\n");
    //serial_print("\r\n All Registers\r\n");
    uint8_t i;
    uint8_t reg_array[11] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x17,0x1C,0x1D};
    for (i = 0; i < 11; i++)
    {
      //serial_print_uint8(reg_array[i]); //serial_print(" : "); nrf_debug_register(reg_array[i]); //serial_print("\r\n");
    }
 }

void nrfDebug(bool debugFlag)
{
  _nrfDebug = debugFlag;
}









// bool nrfSend(const uint8_t *data, uint8_t len)
// {

//   nrfWrite(data,len);
//   nrfStartTransmit();
//   uint32_t currentMs = _Millis();
//   uint32_t prevMs = currentMs;
//   do
//   {
//     if(_nrfIrqState == NRF_SUCCESS)
//     {
//       return true;
//     }
//     else if(_nrfIrqState == NRF_FAIL)
//     {
//       break;
//     }
//     currentMs = _Millis();
//   }while((currentMs - prevMs) < NRF_TRANSMIT_TIMEOUT);
//   return false; 
// }