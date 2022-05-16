#include "device.h"
#if defined(ESP32_V010)
#include "..\..\wifi\all_global.h"

char jsonBuf[JSON_BUFFER_SIZE];
char *jsonBufPtr;
int mid = 1;
bool singlePacket = false; // keep it false to send multiple packet

#endif

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
  #elif defined(ESP32_V010)
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
struct gasSensor_t *pldBufPtr = (struct gasSensor_t *)&pldBuf;
uint8_t payloadCount = 1;
uint8_t *pldPtr; //This will keep track of  read memory until sent 
static uint32_t prevMsAck = 0;


// Task task2(10, &updateDisplay);
// Task task1(DEFAULT_DATA_SAMPLE_SEC, &schemaReadSensors); //send payload triggers after 5 second interval


// #define MEMQ_RING_BUF_LEN  4
// #define MEMQ_FLASH_START_ADDR 0
// #define MEMQ_TOTAL_BUFFER   256
// #define MEMQ_SECTOR_ERASE_SZ  4096
// #define MEMQ_PTR_SAVE_AFTER   10  
void deviceBegin()
{
  #if defined(ESP32_V010) 
  #if !defined(FRAM_V010)
  eepromBegin(EEPROM_SIZE);
  #endif
  #endif
  #if !defined(SOFTWARE_I2C)
  Wire.begin();
  led_begin();
  #endif
  schemaBegin();

  
  // scheduler.addTask(&task1);
  // scheduler.addTask(&task2);

  BUZZER_OUT_MODE();
  BUZZER_OFF();
#if defined(DEVICE_HAS_FLASH_MEMORY)
  flash.begin(SPI_SPEED);
  ringObj.begin(MEMQ_RING_BUF_LEN, sizeof(struct memqPtr_t));
  memqBegin(&memq, MEMQ_FLASH_START_ADDR, sizeof(payload_t), MEMQ_TOTAL_BUFFER);

  memqSetMem(&memq, memReader, memWriter, memEraser, MEMQ_SECTOR_ERASE_SZ);
  // Serial.print("[memq addr]: ");
  // Serial.println((uint32_t) &(memq),HEX);
  // Serial.print("memq->ringPtr addr : ");
  // Serial.println((uint32_t) &(memq.ringPtr), HEX);
  memqSetMemPtr(&memq, memPtrReader, memPtrWriter, MEMQ_PTR_SAVE_AFTER);
  // memq -> attachBusSafety(memq, nrfRestorToRxTx, nrfRxTxToStandy1);

#if defined(FLASH_HEALTH_CHECK)
  memqReset(&memq); 
  flash.memoryCheck(MEMQ_FLASH_START_ADDR,(MEMQ_FLASH_START_ADDR+sizeof(payload_t)*MEMQ_TOTAL_BUFFER));
  memqReset(&memq); 
#endif

  #if defined(DATA_ERASE) || defined(FACTORY_RESET)
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
  jsonBufPtr = jsonBuf;
}


uint8_t *deviceMemRead()
{
  #if defined(STAND_ALONE)
  uint16_t len;
  Serial.println("[MEMREAD]>>");
  pldPtr = memqRead(&memq, (uint8_t*)&pldBuf);
  if (pldPtr != NULL)
  {
    if(pldPtr[0]==11)
    {
      Serial.println("[MEMREAD]..sensordata");
      // printSensor((struct gasSensor_t *)pldPtr);
    }
    else if(pldPtr[0]==23)
    {
      Serial.println("[MEMREAD]..log");
    }
    else if(pldBufPtr->unixTime > second())
    {
      pldPtr = NULL;
    }
    else
    {
      Serial.println("[MEMREAD].Wrong Data");
      pldPtr = NULL;
      // return pldPtr;
    }
  }
     
     
  if(pldPtr != NULL)
  {
    // printBuffer(pldPtr, sizeof(payload_t));   
    len = calculateLen(jsonBuf, jsonBufPtr);
    Serial.print("len of json: "); Serial.println(len);
    if(len <= 1)
    {
      jsonBufPtr++;
      jsonBufPtr = toJsonInit(jsonBufPtr);
      jsonBufPtr = toJson(pldPtr, jsonBufPtr);
    }
    else
    {
      jsonBufPtr = toJson(pldPtr, jsonBufPtr);
    }
  }

  len = calculateLen(jsonBuf, jsonBufPtr);
  
  if(( len > 700 ) || ((pldPtr == NULL) && len > 1 ) || (singlePacket &&(pldPtr != NULL)))
  {
    Serial.print("[len of total json]: "); Serial.println(len);
    jsonBufPtr = toJsonFinal(jsonBufPtr);
    mid++;
    if(mid>10) mid = 1;
    Serial.print("new data... mid: "); Serial.println(mid);
    jsonBuf[0] = (char)mid;
    Serial.print("jsonbuf: ");
    Serial.println(jsonBuf);
    return (uint8_t*)jsonBuf;
  }
  else if(pldPtr == NULL)
  {
    jsonBufPtr = jsonBuf;
    return pldPtr;
  }
  else
  {
    jsonBuf[0] = 0;
    return (uint8_t*)jsonBuf;
  }
  #else
  
  if(pldPtr == NULL)
  {
    // Serial.println(F("Reading from Flash"));
    // pldPtr = memQ.read((uint8_t *)&pldBuf, PAYLOAD_READ_COUNT); // Read from flash
    pldPtr = memqRead(&memq, (uint8_t*)&pldBuf);
    if (pldPtr != NULL)
    {
      if(pldBufPtr->unixTime > second())
      {
        pldPtr = NULL;
        return pldPtr;
      }
      if(pldPtr[0]==11)
      {
        printSensor((struct gasSensor_t *)pldPtr);
      }
      else if(pldPtr[0]==23)
      {
        Serial.print("[MEMREAD]..log");
      }
      else
      {
        Serial.print("[MEMREAD].Wrong Data");
        pldPtr = NULL;
        return pldPtr;
      } 
      // Serial.println(F("Read Mem : New"));
      printBuffer(pldPtr, sizeof(payload_t));
    }
    return pldPtr;
  }
  Serial.println("->Read Mem : Old");
  return pldPtr;
 #endif   
}

#if defined(STAND_ALONE)
bool deviceRfSend(uint8_t *data)
#else
void deviceRfSend(uint8_t *data)
#endif
{
  BUZZER_ON();
  #if defined(STAND_ALONE)
  // Serial.println((char *)data);
  int len = calcLen(data);
  
  // ringqPush(ringqObj,(void*)data,len);
  // if(espDataSendToServer((void*)data,len))
  // attachDataSendToServer(espDataSendToServer);
  if (dataSendToServer((void*)data,len))
  {
    Serial.print("len : ");
    Serial.println(len);
    Serial.println("data sent to server>>");
     BUZZER_OFF();
    prevMsAck = millis();
    return true;
  }
  else
  {
    return false;
  }
  #else
  nrfWrite(data, sizeof(payload_t));
  nrtTxStartTransmission();
  BUZZER_OFF();
  prevMsAck = millis();
  #endif
 
}

int deviceRfAckWait()
{
  // Serial.println(F("Ack wait"));
  #if defined(STAND_ALONE)

 
  attachQos2AckFunc(checkQos2Ack);
 
  // if(midSendFlag == mid)
  // {
    if(checkmqttQos2ack(mid))
    {
      Serial.print("Qos2Ack>>  ");
      Serial.print("mid: "); Serial.println(mid);
      midSendFlag = 0;
      pldPtr = NULL;
      jsonBufPtr = jsonBuf; // jsonBufPtr repointing agian
      memset(jsonBuf, 0, sizeof(jsonBuf));
      return 200;
    }
  // }

  if((millis() - prevMsAck) > 100000)
  {
    Serial.println("ack timeout...>>");
    return 404; 
  }
  
  return -1;
  
  
  
  #else
  bool res =  nrfAck();
  if(res)
  {
    pldPtr = NULL;
    return 200;
  }
  return -1;
  #endif
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

// void updateDataInterval(uint32_t time)
// {
//   task1.setInterval(time);
//   Serial.println(F("------>Sample Interval updated"));
// }



#if defined(DEVICE_HAS_FLASH_MEMORY)
	void memReader(uint32_t addr, uint8_t *buf, uint16_t len)
	{
	  Serial.print(F("<T:"));
	  Serial.print(addr);
	  Serial.println(F(">"));
    memqLockBus(&memq);
	  flash.read(addr, buf, sizeof(payload_t));
    memqUnlockBus(&memq);
    Serial.println("print buffer: ");
    printBuffer((byte *)buf, sizeof(payload_t));
	}

	void memWriter(uint32_t addr, uint8_t *buf, uint16_t len)
	{
	  Serial.print(F("<H:"));
	  Serial.print(addr);
	  Serial.println(F(">"));
    // printBuffer((byte *)buf, sizeof(payload_t));
    memqLockBus(&memq);
	  flash.write(addr, buf, sizeof(payload_t));
    memqUnlockBus(&memq);
	}

	void memEraser(uint32_t addr, uint16_t len)
	{
	  Serial.print(F("Erase Addr: ")); Serial.println(addr);
    memqLockBus(&memq);
	  flash.eraseSector(addr);
    memqUnlockBus(&memq);
  #if defined(ESP_V010)
    delay(10);
  #endif
	  uint32_t curPage = addr >> 8;
    memqLockBus(&memq);
	  flash.dumpPage(curPage, pageBuf);
    memqUnlockBus(&memq);
	}

	void memPtrReader(struct memqPtr_t *ptr)
	{
	  // Serial.println(F("memqPtr>Read"));
	  ringObj.readPacket((byte *)ptr);
    // memqPrintBeginLog(&memq);
	}

	void memPtrWriter(struct memqPtr_t *ptr)
	{
    // memqPrintBeginLog(&memq);
	  // Serial.println(F("memqPtr>Write"));
	  ringObj.savePacket((byte *)ptr);
    // memPtrReader(&memq.ringPtr);
	}
#endif

uint16_t calculateLen(char *start, char *end)
{
  uint16_t len = end - start;
  return len;
}

bool isFlashRunning()
{
  return (flash.readMFID() && 0xEF);
}