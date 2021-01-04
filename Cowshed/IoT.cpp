#include "IoT.h"
#include "Schema.h"
#include "radio.h"

uint8_t *readMem();
void sendNrf(uint8_t *data);
int ackWait();
void printBuffer(byte *buf, byte len);

/*********Flash & MemQ variables**********************/
Flash flash(FLASH_CS);
RingEEPROM myeepRom(0x00);
MemQ memQ(256, 1000);

/**********Async Server Objects*********************/
//AsyncServer server(&memQ);
payload_t pldBuf;
uint8_t payloadCount = 1;


void objectsBegin()
{
//  espSerial.begin(9600);
  /**********MemQ and Flash Begin************/
//  memQ.attachFlash(&flash, (void**)&buffer.flashPtr, sizeof(payload_t), TOTAL_PAYLOAD_BUFFERS / 2);
  memQ.attachFlash(&flash,&_ramQFlash, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER / 2);
  memQ.attachEEPRom(&myeepRom, 4);
  
//  memQ.attachSafetyFuncs(nrfRestorToRxTx,nrfRxTxToStandy1);
  memQ.attachSafetyFuncs(NULL,nrfRxTxToStandy1);
//  memQ.reset();
  /*********Server begin********************/
//  server.setServerCbs(nrfSend, ackWait);
////  server.setSchema(sizeof(payload_t), 1);
//  server.setSchema(payloadBuffer,sizeof(payload_t), 1);
//  server.start();

  xferBegin(readMem,sendNrf,ackWait);
  xferReady();
}

uint8_t *readMem()
{
//  Serial.println(F("Reading mem"));
  uint8_t *p = memQ.read((uint8_t*)&pldBuf, payloadCount);
  if (p !=NULL) printBuffer(p, sizeof(payload_t));
  return p;
}

void sendNrf(uint8_t *data)
{
  Serial.println(F("Sending Via nrf"));
  nrfWrite(data,sizeof(payload_t));
  nrfStartTransmit();
}

int ackWait()
{
  Serial.println(F("Ack wait"));
  if(nrfAck()) return 200; 
  else return -1;
}

void printBuffer(byte *buf, byte len)
{
  Serial.print(F("Buffer: "));
  for (byte i = 0; i < len; i++)
  {
    Serial.print(buf[i]); Serial.print(" ");
  }
  Serial.println();
}
