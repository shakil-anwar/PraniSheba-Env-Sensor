#include "radio.h"

#define TX_PIPE      5
#define QUERY_PIPE   0

void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);

uint8_t pipe0Addr[5] = {201,202,203,204,205};

uint8_t txLsByte;
nrfNodeConfig_t nrfConfig;

// uint8_t pipeAddr[6][5] =
// {
//   {1, 2, 3, 4, 5},
//   {2, 2, 3, 4, 5},
//   {3, 2, 3, 4, 5},
//   {4, 2, 3, 4, 5},
//   {5, 2, 3, 4, 5},
//   {6, 2, 3, 4, 5}
// };
void radio_begin()
{
 
  #if defined(ESP32_V010)
  nrfSetTimers(millis_esp32, second);
  nrfSetPin( NRF_CE_PIN, NRF_CSN_PIN);
  #else
  nrfSetTimers(millis, second);
  nrfSetPin(&NRF_CE_PORT, NRF_CE_PIN, &NRF_CSN_PORT, NRF_CSN_PIN);
  #endif
  nrfSetDebug(true);
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM, SPI_SPEED);
  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
  
  nrfQryObj.pipe = QUERY_PIPE;
  nrfQryObj.activePingAddr = pipe0Addr;
  nrfQryObj.pipe0AddrP = pipe0Addr;
  
  // Serial.println((uint32_t)*pipe0Addr);
  // Serial.println((uint32_t)*(nrfQryObj.activePingAddr));
  
  // nrfQryObj.pipeAddr = &pipeAddr;
  nrfQueryBeginClient(&nrfQryObj);
  // nrfQueryClientSet(QUERY_PIPE, pipe0Addr);
  //nrfQueryBufferSet((uint8_t*)&queryBuffer, sizeof(queryData_t));

  //  nrfSetTx(pipeAddr[TX_PIPE], true);
  nrfPowerDown();
  Serial.println(F("Radio setup done"));
}

// void printradioaddr()
// {
//   Serial.println("call print add");
//    Serial.println(pipe0Addr[0]);
//   Serial.println((uint32_t)*pipe0Addr);
//   Serial.println((uint32_t)*(nrfQryObj.activePingAddr));
// }

void radioStart()
{
  //  nrfStandby1();
  //  nrfTXStart(); //nrf goes standby-1
  pinMode(NRF_IRQ, INPUT_PULLUP);
  #if defined(ESP32_V010)
  attachInterrupt(NRF_IRQ, nrfIrq, FALLING );
  #else
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ), nrfIrq, FALLING );
  #endif
  
  

}
void txIsr(void)
{
//  nrfClearTxDs();
}

void rxIsr(void)
{
  Serial.println(F("RX IRQ"));
//  nrfClearRxDr();
}

void maxRtIsr(void)
{
//  nrfClearMaxRt();
  nrf_flush_rx();
  nrf_flush_tx();

}



