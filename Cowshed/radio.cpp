#include "radio.h"
#include "Schema.h"

#define TX_PIPE      5
#define QUERY_PIPE   1

void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);



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
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM); //radio in power down mode
  
  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
  nrfQueryClientSet(QUERY_PIPE,pipeAddr[TX_PIPE], pipeAddr[QUERY_PIPE]);
  
  nrfSetTx(pipeAddr[TX_PIPE], true);
  nrfPowerDown();
  Serial.println(F("Radio setup done"));
}

void radioStart()
{
  nrfStandby1();
  nrfTXStart(); //nrf goes standby-1
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), nrfIrq, FALLING );
}
void txIsr(void)
{
  nrfClearTxDs();
}

void rxIsr(void)
{
  uint8_t pipe = pipeAvailFast();
  nrf_flush_rx();
  nrfClearRxDr();
}

void maxRtIsr(void)
{
  nrfClearMaxRt();
  nrf_flush_rx();
  nrf_flush_tx();
}

uint32_t getRtcTime()
{
  unixTime_t *uTimePtr = (unixTime_t*)&queryBuffer;
  uTimePtr -> type = 0;
  uTimePtr -> opCode = 1;
  uTimePtr -> time = 0;
  
  nrfStandby1();
  uTimePtr = (unixTime_t*)nrfQuery((void*)&queryBuffer,sizeof(queryData_t));
  nrfPowerDown();
  Serial.print(F("Type : "));Serial.print(uTimePtr -> type);
  Serial.print(F(" Opcode: "));Serial.println(uTimePtr -> opCode);
  delay(2000);
  if(uTimePtr != NULL)
  {
    if(uTimePtr -> type == 0 && uTimePtr -> opCode == 1)
    {
      Serial.print(F("Received Time : "));Serial.println(uTimePtr -> time);
      return (uTimePtr -> time);
    }
  }
  else
  {
    Serial.println(F("RTC Query falied"));
    return 0;
  }
}
