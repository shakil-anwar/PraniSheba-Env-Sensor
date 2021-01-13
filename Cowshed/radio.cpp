#include "radio.h"

#define TX_PIPE      5
#define QUERY_PIPE   1

void txIsr(void);
void rxIsr(void);
void maxRtIsr(void);

uint8_t commonAddr[5] = {1, 2, 3, 4, 5};
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
  nrfSetPin(&NRF_CE_PORT, NRF_CE_PIN, &NRF_CSN_PORT, NRF_CSN_PIN);
//  nrfSetPin(&PORTB,2, &PORTB, 1);
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM, 1000000);
  
  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
//  nrfQueryClientSet(QUERY_PIPE,pipeAddr[QUERY_PIPE]);
  nrfQueryClientSet(QUERY_PIPE,commonAddr);
  nrfQueryBufferSet((uint8_t*)&queryBuffer, sizeof(queryData_t));
  
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
  nrfClearTxDs();
}

void rxIsr(void)
{
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
  memset(&queryBuffer,0, sizeof(queryData_t));
//  uTimePtr -> type = 0;
//  uTimePtr -> opCode = 1;
//  uTimePtr -> utime = 0;
  
  nrfStandby1();
  nrfTXStart();
  uTimePtr = (unixTime_t*)nrfQuery(0,1);
  flash.printBytes((byte*)uTimePtr,sizeof(unixTime_t));
  nrfPowerDown();
  Serial.print(F("Type : "));Serial.print(uTimePtr -> type);
  Serial.print(F(" Opcode: "));Serial.println(uTimePtr -> opCode);
//  delay(2000);
  if(uTimePtr != NULL)
  {
    
    if(uTimePtr -> type == 0 && uTimePtr -> opCode == 1)
    {
      Serial.print(F("Received Time : "));Serial.println(uTimePtr -> utime);
//      Serial.print(F("padding :"));      Serial.println(uTimePtr -> padding);
      return (uTimePtr -> utime);
    }
  }
  else
  {
    Serial.println(F("RTC Query falied"));
    return 0;
  }
}
