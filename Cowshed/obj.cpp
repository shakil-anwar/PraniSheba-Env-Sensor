#include "Obj.h"
#include "pin.h"
#include <SoftwareSerial.h>
#include "dataSchema.h"
#include "radio.h"

#define PIPE_SERVER_SEND_CODE       10

//void pipeSendServer(const char *data);
int ackWait();
//void nrf_ce_enable();
//void nrf_ce_disable();

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
  
  memQ.attachSafetyFuncs(ce_enable,ce_disable);
//  memQ.reset();
  /*********Server begin********************/
  server.setServerCbs(pipeSendServer, ackWait);
  server.setSchema(sizeof(payload_t), 1);
//  server.setJson(toJson, 256);
  server.start();
}


void pipeSendServer(const uint8_t *data, const uint8_t len)
{
  rf_send_success = false;
  retryCount = 0;
//  nrf_send((uint8_t*)data);
  nrf_send_byte(data, len);
}

int ackWait()
{
  if(retryCount == 16)
  {
    return 404;
  }
  else if(retryCount != 16 && rf_send_success == true)
  {
    return 200;
  }else{
    return -1;
  }
}

//void nrf_ce_enable()
//{
//  CE_ENABLE();
//}
//
//void nrf_ce_disable()
//{
//  CE_DISBLE();
//}
