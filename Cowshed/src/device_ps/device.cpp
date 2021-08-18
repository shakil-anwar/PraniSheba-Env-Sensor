#include "device.h"

void memReader(uint32_t addr, uint8_t *buf, uint16_t len);
void memWriter(uint32_t addr, uint8_t *buf, uint16_t len);
void memEraser(uint32_t addr, uint16_t len);
void memPtrReader(struct memqPtr_t *ptr);
void memPtrWriter(struct memqPtr_t *ptr);

#define PAYLOAD_READ_COUNT 1
void printBuffer(byte *buf, byte len);

#define DEFAULT_DATA_SAMPLE_SEC   5

/*********Flash & MemQ variables**********************/

#if defined(DEVICE_HAS_FLASH_MEMORY)
  #if defined(PCB_V_0_1_0)
    Flash flash(FLASH_CS);
  #elif defined(PCB_V_0_2_0)
    Flash flash(FLASH_CS,FLASH_HOLD);
  #endif
// MemQ memQ(1, 100);
// RingEEPROM myeepRom(RING_EEPROM_ADDR);
RingEEPROM ringObj(RING_EEPROM_ADDR);
struct memq_t memq;
uint8_t pageBuf[256];
#else
  #warning "device has no flash memory"
#endif


/**********Async Server Objects*********************/
payload_t pldBuf;
struct sensor_t *pldBufPtr = (struct sensor_t *)&pldBuf;
uint8_t payloadCount = 1;
uint8_t *pldPtr; //This will keep track of  read memory until sent 


Task task2(10, &updateDisplay);
Task task1(DEFAULT_DATA_SAMPLE_SEC, &schemaReadSensors); //send payload triggers after 5 second interval


// #define MEMQ_RING_BUF_LEN  4
// #define MEMQ_FLASH_START_ADDR 0
// #define MEMQ_TOTAL_BUFFER   256
// #define MEMQ_SECTOR_ERASE_SZ  4096
// #define MEMQ_PTR_SAVE_AFTER   10  
void deviceBegin()
{
  schemaBegin();
  led_begin();
  scheduler.addTask(&task1);
  scheduler.addTask(&task2);

  BUZZER_OUT_MODE();
  BUZZER_OFF();
#if defined(DEVICE_HAS_FLASH_MEMORY)
  flash.begin(SPI_SPEED);
  ringObj.begin(MEMQ_RING_BUF_LEN, sizeof(struct memqPtr_t));
  memqBegin(&memq, 12288, sizeof(payload_t), MEMQ_TOTAL_BUFFER);

  memqSetMem(&memq, memReader, memWriter, memEraser, MEMQ_SECTOR_ERASE_SZ);
  memqSetMemPtr(&memq, memPtrReader, memPtrWriter, MEMQ_PTR_SAVE_AFTER);
  // memq -> attachBusSafety(memq, nrfRestorToRxTx, nrfRxTxToStandy1);

  #if defined(DATA_ERASE)
  memqReset(&memq); 
  #endif

  // memQ.attachFlash(&flash, &_ramQFlash, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER / 2);
  // memQ.attachEEPRom(&myeepRom, 4);
  // //  memQ.attachSafetyFuncs(nrfRestorToRxTx,nrfRxTxToStandy1);
  // memQ.attachSafetyFuncs(NULL, nrfRxTxToStandy1);
  // #if defined(DATA_ERASE)
  // // memQ.reset();
  // memQ.erase();
  // #endif
  // memQ.debug(true);
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
    // pldPtr = memQ.read((uint8_t *)&pldBuf, PAYLOAD_READ_COUNT); // Read from flash
    pldPtr = memqRead(&memq, (uint8_t*)&pldBuf);
    if (pldPtr != NULL)
    {
      if(pldBufPtr->unixTime > second())
      {
        return NULL;
      }
      // Serial.println(F("Read Mem : New"));
      printBuffer(pldPtr, sizeof(payload_t));
    }
    return pldPtr;
  }
  Serial.println("->Read Mem : Old");
  return pldPtr;
}

void deviceRfSend(uint8_t *data)
{
  // Serial.println(F("Sending Via nrf"));
  BUZZER_ON();
  nrfWrite(data, sizeof(payload_t));
  nrtTxStartTransmission();
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



#if defined(DEVICE_HAS_FLASH_MEMORY)
	void memReader(uint32_t addr, uint8_t *buf, uint16_t len)
	{
	  Serial.print(F("<====Tail :"));
	  Serial.print(addr);
	  Serial.println(F("====>"));
	  flash.read(addr, buf, sizeof(payload_t));
	}

	void memWriter(uint32_t addr, uint8_t *buf, uint16_t len)
	{
	  Serial.print(F("<====Head :"));
	  Serial.print(addr);
	  Serial.println(F("====>"));
	  flash.write(addr, buf, sizeof(payload_t));
	}

	void memEraser(uint32_t addr, uint16_t len)
	{
	  Serial.print(F("Erasing Addr : ")); Serial.println(addr);
	  flash.eraseSector(addr);
	  uint32_t curPage = addr >> 8;
	  flash.dumpPage(curPage, pageBuf);
	}

	void memPtrReader(struct memqPtr_t *ptr)
	{
	  Serial.println(F("memqPtr>Reader"));
	  ringObj.readPacket((byte *)ptr);
	}

	void memPtrWriter(struct memqPtr_t *ptr)
	{
	  Serial.println(F("memqPtr>Writer"));
	  ringObj.savePacket((byte *)ptr);
	}
#endif