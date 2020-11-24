#include "Obj.h"
#include "pin.h"
#include <SoftwareSerial.h>
#include "dataSchema.h"
#include "radio.h"

#define PIPE_SERVER_SEND_CODE       10

void pipeSendServer(const uint8_t *data, const uint8_t len);
int ackWait();

/*********Flash & MemQ Library**********************/
Flash flash(FLASH_CS);
RingEEPROM myeepRom(0x00);
MemQ memQ(256, 1000);

/**********Async Server Objects*********************/
AsyncServer server(&memQ);


void objectsBegin()
{
//  espSerial.begin(9600);
  /**********MemQ and Flash Begin************/
  memQ.attachFlash(&flash, (void**)&buffer.flashPtr, sizeof(payload_t), TOTAL_PAYLOAD_BUFFERS / 2);
  memQ.attachEEPRom(&myeepRom, 4);
  
//  memQ.attachSafetyFuncs(nrfRestorToRxTx,nrfRxTxToStandy1);
  memQ.attachSafetyFuncs(NULL,nrfRxTxToStandy1);
//  memQ.reset();
  /*********Server begin********************/
  server.setServerCbs(pipeSendServer, ackWait);
  server.setSchema(sizeof(payload_t), 1);
  server.start();
}


void pipeSendServer(const uint8_t *data, const uint8_t len)
{
  nrf_flush_tx();
  nrfWrite(data,len);
  nrfStartTransmit();
}

int ackWait()
{
  int waitCount = 10;
  nrf_irq_state_t irqState;
  do
  {
    irqState = waitAck();
    if(irqState == NRF_SUCCESS)
    {
      return 200;
    }
    else if(irqState == NRF_FAIL)
    {
      break;
    }
   delay(1);
  }while(--waitCount);
  return -1;
}
