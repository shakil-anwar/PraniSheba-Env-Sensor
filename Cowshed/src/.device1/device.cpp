#include "device.h"


void printBuffer(byte *buf, byte len);



/*********Flash & MemQ variables**********************/

#if defined(DEVICE_HAS_FLASH_MEMORY)
Flash flash(FLASH_CS);
MemQ memQ(256, 1000);
RingEEPROM myeepRom(0x00);
#else
	#warning "device has no flash memory"
#endif


/**********Async Server Objects*********************/
payload_t pldBuf;
uint8_t payloadCount = 1;



Task task1(DATA_ACQUIRE_INTERVAL, &schemaReadSensors); //send payload triggers after 5 second interval
// Task task2(10, &updateDisplay);

void deviceBegin()
{
  schemaBegin();
  scheduler.addTask(&task1);
  // scheduler.addTask(&task2);

#if defined(DEVICE_HAS_FLASH_MEMORY)
  memQ.attachFlash(&flash, &_ramQFlash, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER / 2);
  memQ.attachEEPRom(&myeepRom, 4);
  //  memQ.attachSafetyFuncs(nrfRestorToRxTx,nrfRxTxToStandy1);
  memQ.attachSafetyFuncs(NULL, nrfRxTxToStandy1);
  //  memQ.reset();
#endif
  sensorBegin();
  sensorCalibrate();
}

uint8_t *deviceMemRead()
{
  //  Serial.println(F("Reading mem"));
  /**********Read from Flash****************/
//    uint8_t *p = memQ.read((uint8_t*)&pldBuf, payloadCount);// Read from flash
  /**********Read from Flash Memory*********/
  uint8_t *p = ramQRead();
  ramQUpdateTail();
//  Serial.print(F("RamQ Head Counter : "));Serial.println(ramQCounter);
//  Serial.print(F("RamQ Tail Counter : "));Serial.println(ramQTailCounter);
  
  if (p != NULL) 
  {
    Serial.println(F("Mem Read Ok"));
    printBuffer(p, sizeof(payload_t));
  }
  return p;
}

void deviceRfSend(uint8_t *data)
{
  Serial.println(F("Sending Via nrf"));
  nrfWrite(data, sizeof(payload_t));
  nrfStartTransmit();
}

int deviceRfAckWait()
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