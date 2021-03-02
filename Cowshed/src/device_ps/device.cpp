#include "device.h"

#define PAYLOAD_READ_COUNT 1
void printBuffer(byte *buf, byte len);



/*********Flash & MemQ variables**********************/

#if defined(DEVICE_HAS_FLASH_MEMORY)
  #if defined(PCB_V_0_1_0)
    Flash flash(FLASH_CS);
  #elif defined(PCB_V_0_2_0)
    Flash flash(FLASH_CS,FLASH_HOLD);
  #endif
MemQ memQ(1, 100);
RingEEPROM myeepRom(RING_EEPROM_ADDR);
#else
  #warning "device has no flash memory"
#endif


/**********Async Server Objects*********************/
payload_t pldBuf;
uint8_t payloadCount = 1;
uint8_t *pldPtr; //This will keep track of  read memory until sent 


Task task2(10, &updateDisplay);
Task task1(DATA_ACQUIRE_INTERVAL, &schemaReadSensors); //send payload triggers after 5 second interval


void deviceBegin()
{
  schemaBegin();
  led_begin();
  scheduler.addTask(&task1);
  scheduler.addTask(&task2);

  BUZZER_OUT_MODE();
  BUZZER_OFF();
#if defined(DEVICE_HAS_FLASH_MEMORY)
  memQ.attachFlash(&flash, &_ramQFlash, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER / 2);
  memQ.attachEEPRom(&myeepRom, 4);
  //  memQ.attachSafetyFuncs(nrfRestorToRxTx,nrfRxTxToStandy1);
  memQ.attachSafetyFuncs(NULL, nrfRxTxToStandy1);
  #if defined(FACTORY_RESET)
  // memQ.reset();
  memQ.erase();
  #endif
  memQ.debug(true);
#endif
  sensorBegin();

  pldPtr = NULL; //This has to be null for proper operation.
}


uint8_t *deviceMemRead()
{
  //  Serial.println(F("Reading mem"));
  /**********Read from Flash****************/
//    uint8_t *p = memQ.read((uint8_t*)&pldBuf, payloadCount);// Read from flash
  /**********Read from Flash Memory*********/
//   uint8_t *p = ramQRead();
//   ramQUpdateTail();
// //  Serial.print(F("RamQ Head Counter : "));Serial.println(ramQCounter);
// //  Serial.print(F("RamQ Tail Counter : "));Serial.println(ramQTailCounter);
  
//   if (p != NULL) printBuffer(p, sizeof(payload_t));
//   return p;

  if(pldPtr == NULL)
  {
    // Serial.println(F("Reading from Flash"));
    pldPtr = memQ.read((uint8_t *)&pldBuf, PAYLOAD_READ_COUNT); // Read from flash
    if (pldPtr != NULL)
    {
      // Serial.println(F("Read Mem : New"));
      printBuffer(pldPtr, sizeof(payload_t));
    }
    return pldPtr;
  }
  Serial.println(F("----->Read Mem : Old"));
  return pldPtr;
}

void deviceRfSend(uint8_t *data)
{
  // Serial.println(F("Sending Via nrf"));
  BUZZER_ON();
  nrfWrite(data, sizeof(payload_t));
  nrfStartTransmit();
  BUZZER_OFF();
}

int deviceRfAckWait()
{
  // Serial.println(F("Ack wait"));
  bool res =  nrfAck();
  if(res)
  {
    pldPtr = NULL;
    return 200;
  }
  return -1;
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

void updateDataInterval(uint32_t time)
{
  task1.setInterval(time);
  Serial.println(F("-------------->Sample Interval updated"));
}