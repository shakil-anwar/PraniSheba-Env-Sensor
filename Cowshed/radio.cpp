#include "radio.h"
#include "EEPROM.h"
#define TX_PIPE      5
#define QUERY_PIPE   0

void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);

uint8_t pipe0Addr[5] = {120, 121, 123, 124, 125};
uint8_t txLsByte;

uint8_t pipeAddr[6][5] =
{
  {1, 2, 3, 4, 5},
  {2, 2, 3, 4, 5},
  {3, 2, 3, 4, 5},
  {4, 2, 3, 4, 5},
  {5, 2, 3, 4, 5},
  {6, 2, 3, 4, 5}
};
void radio_begin()
{
  nrfSetTimers(millis, second);
  nrfSetPin(&NRF_CE_PORT, NRF_CE_PIN, &NRF_CSN_PORT, NRF_CSN_PIN);
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM, SPI_SPEED);

  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
  nrfQueryClientSet(QUERY_PIPE, pipe0Addr);
  //nrfQueryBufferSet((uint8_t*)&queryBuffer, sizeof(queryData_t));

  //  nrfSetTx(pipeAddr[TX_PIPE], true);
  nrfPowerDown();
  Serial.println(F("Radio setup done"));
}

void radioStart()
{
  //  nrfStandby1();
  //  nrfTXStart(); //nrf goes standby-1
  pinMode(NRF_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NRF_IRQ), nrfIrq, FALLING );
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

void saveAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Saving.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for (uint8_t i = 0; i < sizeof(addr_t); i++)
  {
    EEPROM.update(ROM_ADDR_FOR_TXD + i, *(ptr + i));
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}

void readAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Reading.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for (uint8_t i = 0 ; i < sizeof(addr_t); i++)
  {
    *(ptr + i) = EEPROM.read(ROM_ADDR_FOR_TXD + i);
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}
