#include "Schema.h"
#include "IoT.h"
#include "led.h"


volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
queryData_t queryBuffer;

sensor_t sensor;

void dataSchemaBegin()
{
  /**************Compile time checking************************/
  static_assert(!(TOTAL_PAYLOAD_BUFFER % 2), "\"TOTAL_FLASH_BUFFERS\" Should be Even Number");
  
  /**************Done checking***********************************/

  ramQSet(&payload, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER);
  //Initialise sensor constant parameter
//  sensor.type  = SENSOR_TYPE;
//  sensor.id = COWSHED_ID;
}


sensor_t *getSensorsData(sensor_t *senPtr)
{
  sensor_t *sensor = senPtr;

  sensor -> type = SENSOR_TYPE;
  sensor -> id = COWSHED_ID;
  sensor -> unixTime = second();
  sensor -> temp = getTemp();
  sensor -> hum = getHum();
  sensor -> ammonia = getAmmonia();
  sensor -> methane = getMethane();
  return senPtr;
}

void printSensor(sensor_t *sensor)
{
  Serial.println(F("<---------Sensor Data----------------->"));
  Serial.print(F("Time : ")); Serial.println(sensor -> unixTime );
  Serial.print(F("ID : ")); Serial.println(sensor -> id );
  Serial.print(F("Temperature : ")); Serial.print(sensor -> temp ); Serial.println(" C");
  Serial.print(F("Humidity : ")); Serial.print(sensor -> hum ); Serial.println(" %Rh");
  Serial.print(F("Ammonia : ")); Serial.print(sensor -> ammonia ); Serial.println(" ppm");
  Serial.print(F("Methane : ")); Serial.print(sensor -> methane ); Serial.println(" ppm");
}


void dataAcquisition()
{
  sensor_t *senPtr = (sensor_t*)ramQHead();
  getSensorsData(senPtr);
  printSensor(senPtr);
  Serial.print(F("ramQ Counter :"));Serial.println(ramQCounter);
  
  ramQUpdateHead();
}

void updateDisplay()
{
  sensor_t *sensorPtr = getSensorsData(&sensor);
//  printSensor(sensorPtr);
  uint8_t level;
  //humidity
  level = (uint8_t)sensorPtr -> hum;
  level = level / 20;
  led_set_level(1, level) ;

  //Temperature
  level = (uint8_t)sensorPtr -> temp;
  level = level / 10;
  led_set_level(2, level);

  //Methane
  level = (uint8_t)sensorPtr -> methane;
  level = level / 5;
  led_set_level(3, level);

  //Ammonia
  level = (uint8_t)sensorPtr -> ammonia;
  level = level / 10;
  led_set_level(4, level);
//  readPayload();
}


//void readPayload()
//{
//  Serial.print(F("Payload Receiving:  ")); Serial.println(second());
//  uint8_t *payload = (uint8_t*)&buffer.nrfPtr[buffer.pIndex];
//  sensor_t *sensorPtr = getSensorsData();
//  memset(payload, '\0', MAX_PAYLOAD_BYTES);
//  memcpy(payload, (uint8_t*)sensorPtr, sizeof(sensor_t));
//
//  sensor_t *bptr = (sensor_t*)&buffer.nrfPtr[buffer.pIndex];
//
//  Serial.print(F("bufIndex :")); Serial.print(buffer.tIndex);
//  Serial.print(F(" | index :")); Serial.print(buffer.pIndex);
//  Serial.print(F(" | DataIndex :")); Serial.println(bptr -> id);
//
//  buffer.pIndex++;
//  buffer.tIndex++;
//  if (buffer.tIndex >= TOTAL_PAYLOAD_BUFFERS / 2)
//  {
//    if (buffer.tIndex == TOTAL_PAYLOAD_BUFFERS / 2)
//    {
//      buffer.nrfPtr   = (payload_t*)&buffer.payload[TOTAL_PAYLOAD_BUFFERS / 2];
//      buffer.flashPtr = (payload_t*)&buffer.payload[0];
//      buffer.pIndex = 0;
//    }
//    else if (buffer.tIndex == TOTAL_PAYLOAD_BUFFERS)
//    {
//      buffer.nrfPtr   = (payload_t*)&buffer.payload[0];
//      buffer.flashPtr = (payload_t*)&buffer.payload[TOTAL_PAYLOAD_BUFFERS / 2];
//      buffer.pIndex   = 0;
//      buffer.tIndex   = 0;
//    }
//  }
//  Serial.println("");
//  for (int j = 0; j < MAX_PAYLOAD_BYTES ; j++) {
//    Serial.print((char)payload[j], HEX);
//    Serial.print(" ");
//  }
//  Serial.println("");
//
//  //  nrf_send(payload);
//
//  //  printSensor(sensorPtr);
//  //handle nrf data sending here
//}
