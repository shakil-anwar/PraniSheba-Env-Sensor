#include "Sensors.h"
#include <assert.h>

#define HEAD_PTR 1
#define TAIL_PTR 2

volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
// queryData_t queryBuffer;

struct gasSensor_t sensor;

uint8_t checksumCalc(uint8_t *buf,uint8_t len);

void schemaBegin()
{
  /**************Compile time checking************************/
  static_assert(!(TOTAL_PAYLOAD_BUFFER % 2), "\"TOTAL_FLASH_BUFFERS\" Should be Even Number");
  
  /**************Done checking***********************************/
  // void *payldPtr = (void *)&payload;
  ramQSet((void *)&payload, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER);
  //Initialise sensor constant parameter
//  sensor.type  = SENSOR_TYPE;
//  sensor.id = COWSHED_ID;
}

uint8_t checksumCalc(uint8_t *buf,uint8_t len)
{
  struct header_t *hPtr = (struct header_t*)buf;
  hPtr->checksum = 0;

  uint16_t sum = 0;
  uint8_t *ptr = buf;
  uint8_t i;
  for(i = 0; i< len; i++)
  {
    sum += (uint16_t)ptr[i];
  }
  return (uint8_t)sum;
}

struct  gasSensor_t *getSensorsData(struct gasSensor_t *senPtr)
{
  struct gasSensor_t *sensor = senPtr;
  sensor -> header.type = SENSOR_TYPE;
  sensor -> header.id = config.deviceId;
  sensor -> unixTime = second();
  sensor -> temp = (getTemp() - 2);
  sensor -> hum = getHum();
  if(sensor -> hum >100)
  {
    sensor -> hum = 98.18;
  }
  sensor -> ammonia = getAmmonia();
  sensor -> methane = getMethane();
#if defined(DEVICE_HAS_SMOKE_SENSOR)
  sensor -> smoke = 0.0;
#else
  sensor -> isServerSynced = 255;
#endif
  sensor -> header.checksum = 0;

  //calculate checksum
  sensor -> header.checksum = checksumCalc((uint8_t*)sensor,sizeof(struct gasSensor_t));
  return senPtr;
}

void printSensor(struct gasSensor_t *sensor)
{
  Serial.println(F("<-----Sensor Data----->"));
  Serial.print(F("Time : ")); Serial.println(sensor -> unixTime );
  Serial.print(F("ID : ")); Serial.println(sensor -> header.id );
  Serial.print(F("Temp: ")); Serial.print(sensor -> temp ); Serial.println(" C");
  Serial.print(F("Hum: ")); Serial.print(sensor -> hum ); Serial.println(" %Rh");
  Serial.print(F("NH3: ")); Serial.print(sensor -> ammonia ); Serial.println(" ppm");
  Serial.print(F("CH4: ")); Serial.print(sensor -> methane ); Serial.println(" ppm");
#if defined(DEVICE_HAS_SMOKE_SENSOR)
  Serial.print(F("SMOKE: ")); Serial.print(sensor -> smoke ); Serial.println("");
#endif
  Serial.print(F("cksum: ")); Serial.println(sensor->header.checksum );
}


void memqSave()
{
  //When ramq full, _ramQBase points to the base address,
  // Serial.println(F("-----memqSave Called---"));
  if (_ramQBase != NULL)
  {
    // Serial.println(F("-----_ramQBase != NULL---"));
    uint8_t *ramqPtr = (uint8_t*)_ramQBase;
    for (uint16_t i = 0; i < TOTAL_PAYLOAD_BUFFER; i++)
    {
      memqWrite(&memq, ramqPtr);
      //uint32_t curPage = (memq ->ringPtr._head) >> 8;
      //flash.dumpPage(curPage, pageBuf);
      //Serial.print(F("Counter : ")); Serial.println(memq -> _ptrEventCounter);
      ramqPtr += sizeof(payload_t);
    }
    _ramQBase = NULL; //null pagePtr to avoid overwrite
  }
}

void schemaReadSensors()
{
  struct gasSensor_t *senPtr = (struct gasSensor_t*)ramQHead();
  getSensorsData(senPtr);
  printSensor(senPtr);
  Serial.print(F("ramQ Counter :"));Serial.println(ramQCounter);
  
//  nrfSend((const uint8_t*)senPtr,sizeof(payload_t));
//  server.printPayload((byte*)senPtr,sizeof(payload_t));
  ramQUpdateHead();
  memqSave();
  Serial.print(F("_ptrEventCounter :"));Serial.println(memq._ptrEventCounter);
  Serial.print(F("_maxPtrEvent :"));Serial.println(memq._maxPtrEvent);
}


struct payloadLog_t
{
  uint8_t type;
  uint8_t rcvChecksum;
  uint8_t calcChecksum;
  bool isChecksumOk;
  bool isTypDefined;
  bool isPktHealthy;
};

void payloadPrintLog(struct payloadLog_t *pldLog);
// char *bolusToJson(struct bolusXYZ_t *bolus, char *buffer, uint8_t headTailPtr = HEAD_PTR );
// char *bolusGyroToJson(struct bolusGyro_t *bolusGyro, char *buffer, uint8_t headTailPtr = HEAD_PTR );
// char *bolusTempToJson(struct bolusTemp_t *bolusTempPtr, char *buffer, uint8_t headTailPtr = HEAD_PTR );
char *gasSensorToJson(struct gasSensor_t* gasSensor, char *buffer, uint8_t headTailPtr = HEAD_PTR );
char *gasSensorLogToJson(struct gasSensorLog_t* gasSensor, char *buffer, uint8_t headTailPtr = HEAD_PTR );


/**********Global variable for Payload Buffering ************/




char *gasSensorToJson(struct gasSensor_t* gasSensor, char *buffer, uint8_t headTailPtr)
{
  char *json = buffer;
  if(gasSensor -> unixTime < second())
  {  
    // *json = '[';              json++;
    *json = '[';              json++;
    // *json = GASSENSOR_TYPE;   json++;
    // *json = ',';              json ++;
    json = add_int(json, (int16_t)GASSENSOR_TYPE);
    *json = '[';              json++;
    /*************ID************************/
    *json = '\"';             json ++;
    *json = GAS_ID_PREFIX;    json++;
    json = add_int(json, (int16_t)gasSensor ->header.id);
    json--;
    *json = '\"'; json ++;
    *json = ','; json ++;
    /************Time***********************/
    
    json = add_ulong(json, gasSensor -> unixTime);
    
    json--;
    *json = ']'; json++;
    *json = ','; json++;
    /************Sensor data****************/
    *json = '['; json++;
    json = add_float(json, gasSensor -> ammonia);
    json = add_float(json, gasSensor -> methane);
  #if defined(DEVICE_HAS_SMOKE_SENSOR)
    json = add_float(json, gasSensor -> smoke);
  #endif  //DEVICE_HAS_SMOKE_SENSOR
    json = add_float(json, gasSensor -> hum);
    json = add_float(json, gasSensor -> temp);
    json--;
    *json = ']'; json++;
    *json = ']'; json++;
    // *json = ']'; json++;
  }else{
    *json = '\0'; json++;
    return json;
  }

  if (headTailPtr == HEAD_PTR)
  {
    
    *json = '\0'; json++;
    *json = '\0';
    return buffer;
  }
  else
  {
    *json = ','; json++;
    *json = '\0';
    return json;
  }
}

char *gasSensorLogToJson(struct gasSensorLog_t* sensorLog, char *buffer, uint8_t headTailPtr)
{
  char *json = buffer;
  // *json = '[';              json++;
  *json = '[';              json++;
  // *json = GASSENSOR_LOG_TYPE;   json++;
  json = add_int(json, (int16_t)GASSENSOR_LOG_TYPE);
  // *json = ',';              json ++;
  *json = '{';              json++;
  /*************ID************************/
  strncpy(json,"\"id\":",5);
  // *json = '\"';             json ++;
  // *json = 'i';             json ++;
  // *json = 'd';             json ++;
  // *json = '\"';             json ++;
  // *json = ':';             json ++;
  json += 5;
  *json = '\"';             json ++;
  *json = GAS_ID_PREFIX;    json++;
  json = add_int(json, (int16_t)sensorLog ->header.id);
  json--;
  *json = '\"'; json ++;
  *json = ','; json ++;
  /************Time***********************/
  strncpy(json,"\"ts\":",5);
  json += 5;
  *json = '\"';             json ++;
  json = add_ulong(json, sensorLog -> unixTime);
  json--;
  *json = '\"'; json ++;
  *json = ','; json++;
  /************Sensor data****************/
  strncpy(json,"\"data\":",7);
  json += 7;
  *json = '['; json++;
  json = add_int(json, sensorLog -> errorCode);
  json = add_int(json, sensorLog -> hardwareErrorCode);
  json = add_uint(json, sensorLog -> restartCount);
  json = add_uint(json, sensorLog -> slotMissed);
  json = add_int(json, sensorLog -> samplingFreq);
  json = add_ulong(json, sensorLog -> flashAvailablePackets);
  json = add_float(json, sensorLog -> railVoltage);
  json--;
  *json = ']'; json++;
  *json = '}'; json++;
  *json = ']'; json++;
  // *json = ']'; json++;

  if (headTailPtr == HEAD_PTR)
  {
    
    *json = '\0'; json++;
    *json = '\0';
    return buffer;
  }
  else
  {
    *json = ','; json++;
    *json = '\0';
    return json;
  }

}



void payloadPrintLog(struct payloadLog_t *pldLog)
{
  Serial.print(F("PLD->Type:"));Serial.print(pldLog->type);
  Serial.print(F("|Rcv cSum:")); Serial.print(pldLog->rcvChecksum);
  Serial.print(F("|Calc cSum:")); Serial.print(pldLog->calcChecksum);
  // Serial.print(F("|cSumOk:")); Serial.print(pldLog->isChecksumOk);
  Serial.print(F("|typeDef: ")); Serial.println(pldLog->isTypDefined);
}

char *toJsonInit(char *buffer)
{
  Serial.println("json intiallized......");
  char *jsonPtr = buffer;
  *jsonPtr = '['; jsonPtr++;
  return jsonPtr;
}

char *toJsonFinal(char *buffer)
{
  char *jsonPtr = buffer;
  jsonPtr--;
  *jsonPtr = ']'; jsonPtr++;
  *jsonPtr = '\0';
  return jsonPtr;
}


char *toJson( uint8_t *payloadP, char *buffer, uint8_t totalPayload)
{
  char *jsonPtr = buffer;
  uint8_t *payloadPtr = payloadP;

  struct payloadLog_t pldMeta;

  if (totalPayload > 1)
  {
    *jsonPtr = '['; jsonPtr++;
  }
  uint8_t pCount = 0;
  do
  {
    struct header_t *headerPtr = (struct header_t*)payloadP;
    pldMeta.type = headerPtr->type;
    pldMeta.rcvChecksum = headerPtr->checksum;
    
    

    switch(headerPtr->type)
    {
      
      case 11:
        pldMeta.calcChecksum = checksumCalc(payloadPtr, sizeof(gasSensor_t));
        break;      
      case 23:
        pldMeta.calcChecksum = checksumCalc(payloadPtr, sizeof(gasSensorLog_t));
        break;
      default:
        pldMeta.calcChecksum = checksumCalc(payloadPtr, sizeof(payload_t));
    }
    pldMeta.isChecksumOk = pldMeta.rcvChecksum == pldMeta.calcChecksum;
    
    pldMeta.isTypDefined = true;
    if(pldMeta.isChecksumOk)
    {
      switch (headerPtr->type)
      {
        case 11:
          jsonPtr = gasSensorToJson((struct gasSensor_t*)payloadPtr, jsonPtr, TAIL_PTR);
          break;
        case 23:
          // Serial.println("Gas Sensor Log");
          jsonPtr = gasSensorLogToJson((struct gasSensorLog_t*)payloadPtr, jsonPtr, TAIL_PTR);
          break;
        default:
          pldMeta.isTypDefined = false;
          return buffer;
          break;
      }
    }
    else
    {
      payloadPrintLog(&pldMeta);
      return buffer;
    }
    
  //print packet log
    payloadPrintLog(&pldMeta);
    payloadPtr += sizeof(payload_t);

  } while (++pCount < totalPayload);

  
  return jsonPtr;
}



void printPayload(payload_t *payload)
{
  uint8_t *type = (uint8_t*)payload;
  switch (*type)
  {
    // case 1:
    //   Serial.println(F("Type 1"));
    //   printBolus((struct bolusXYZ_t*)payload);
    //   break;
    case 2:
      Serial.println(F("Type 2"));
      printGasSensor((struct gasSensor_t*)payload);
      break;
    default:
      Serial.print(F("Payload_t type not supported"));
  }
}
