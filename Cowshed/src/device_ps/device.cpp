#include "device.h"


void printBuffer(byte *buf, byte len);



/*********Flash & MemQ variables**********************/
Flash flash(FLASH_CS);
RingEEPROM myeepRom(0x00);
MemQ memQ(256, 1000);

/**********Async Server Objects*********************/
//AsyncServer server(&memQ);
payload_t pldBuf;
uint8_t payloadCount = 1;


Task task2(10, &updateDisplay);
Task task1(DATA_ACQUIRE_INTERVAL, &dataAcquisition); //send payload triggers after 5 second interval


void deviceBegin()
{
  led_begin();
  scheduler.addTask(&task1);
  scheduler.addTask(&task2);
	  //  espSerial.begin(9600);
  /**********MemQ and Flash Begin************/
  //  memQ.attachFlash(&flash, (void**)&buffer.flashPtr, sizeof(payload_t), TOTAL_PAYLOAD_BUFFERS / 2);
  memQ.attachFlash(&flash, &_ramQFlash, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER / 2);
  memQ.attachEEPRom(&myeepRom, 4);

  //  memQ.attachSafetyFuncs(nrfRestorToRxTx,nrfRxTxToStandy1);
  memQ.attachSafetyFuncs(NULL, nrfRxTxToStandy1);
  //  memQ.reset();
  /*********Server begin********************/
  //  server.setServerCbs(nrfSend, ackWait);
  ////  server.setSchema(sizeof(payload_t), 1);
  //  server.setSchema(payloadBuffer,sizeof(payload_t), 1);
  //  server.start();
}

uint8_t *readMem()
{
  //  Serial.println(F("Reading mem"));
  /**********Read from Flash****************/
//    uint8_t *p = memQ.read((uint8_t*)&pldBuf, payloadCount);// Read from flash
  /**********Read from Flash Memory*********/
  uint8_t *p = ramQRead();
  ramQUpdateTail();
//  Serial.print(F("RamQ Head Counter : "));Serial.println(ramQCounter);
//  Serial.print(F("RamQ Tail Counter : "));Serial.println(ramQTailCounter);
  
  if (p != NULL) printBuffer(p, sizeof(payload_t));
  return p;
}

void sendNrf(uint8_t *data)
{
  Serial.println(F("Sending Via nrf"));
  nrfWrite(data, sizeof(payload_t));
  nrfStartTransmit();
}

int ackWait()
{
  Serial.println(F("Ack wait"));
  if (nrfAck()) return 200;
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