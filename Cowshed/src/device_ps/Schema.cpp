#include "Sensors.h"

volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
queryData_t queryBuffer;

struct sensor_t sensor;

uint8_t checksumCalc(uint8_t *buf,uint8_t len);

void schemaBegin()
{
  /**************Compile time checking************************/
  static_assert(!(TOTAL_PAYLOAD_BUFFER % 2), "\"TOTAL_FLASH_BUFFERS\" Should be Even Number");
  
  /**************Done checking***********************************/

  ramQSet(&payload, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER);
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

struct  sensor_t *getSensorsData(struct sensor_t *senPtr)
{
  struct sensor_t *sensor = senPtr;

  sensor -> header.type = SENSOR_TYPE;
  sensor -> header.id = config.deviceId;
  sensor -> unixTime = second();
  sensor -> temp = (getTemp() - 2);
  sensor -> hum = getHum();
  sensor -> ammonia = getAmmonia();
  sensor -> methane = getMethane();

  //calculate checksum
  sensor -> header.checksum = checksumCalc((uint8_t*)sensor,sizeof(struct sensor_t));
  return senPtr;
}

void printSensor(struct sensor_t *sensor)
{
  Serial.println(F("<---------Sensor Data----------------->"));
  Serial.print(F("Time : ")); Serial.println(sensor -> unixTime );
  Serial.print(F("ID : ")); Serial.println(sensor -> header.id );
  Serial.print(F("Temperature : ")); Serial.print(sensor -> temp ); Serial.println(" C");
  Serial.print(F("Humidity : ")); Serial.print(sensor -> hum ); Serial.println(" %Rh");
  Serial.print(F("Ammonia : ")); Serial.print(sensor -> ammonia ); Serial.println(" ppm");
  Serial.print(F("Methane : ")); Serial.print(sensor -> methane ); Serial.println(" ppm");
  Serial.print(F("checksum : ")); Serial.println(sensor->header.checksum );
}


void memqSave()
{
  //When ramq full, _ramQBase points to the base address,

  if (_ramQBase != NULL)
  {
    //Serial.println(F("-----_ramQBase != NULL---"));
    uint8_t *ramqPtr = _ramQBase;
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
  struct sensor_t *senPtr = (struct sensor_t*)ramQHead();
  getSensorsData(senPtr);
  printSensor(senPtr);
  Serial.print(F("ramQ Counter :"));Serial.println(ramQCounter);
  
//  nrfSend((const uint8_t*)senPtr,sizeof(payload_t));
//  server.printPayload((byte*)senPtr,sizeof(payload_t));
  ramQUpdateHead();
  memqSave();
}

