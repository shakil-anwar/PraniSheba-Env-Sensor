#include "radio.h"
#include "pin.h"
#include "dataSchema.h"
#include "querySchema.h"
#include "Obj.h"

#define TX_PIPE      1
#define QUERY_PIPE   5

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
  nrfBegin(SPEED_2MB, POWER_ZERO_DBM);
  
  nrfSetIrqs(txIsr, rxIsr, maxRtIsr);
  nrfSetQueryClient(QUERY_PIPE, pipeAddr[TX_PIPE], pipeAddr[QUERY_PIPE]);
  nrfSetTx(pipeAddr[TX_PIPE], true);
  
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
  uTimePtr = (unixTime_t*)nrfQuery((void*)&queryBuffer,sizeof(queryData_t));
  if(uTimePtr != NULL)
  {
    Serial.print(F("Received Time : "));Serial.println(uTimePtr -> time);
  }
  else
  {
    Serial.println(F("RTC Query falied"));
  }
}
