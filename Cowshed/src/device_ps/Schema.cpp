#include "Sensors.h"

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
  sensor -> id = config.deviceId;
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
  
//  nrfSend((const uint8_t*)senPtr,sizeof(payload_t));
//  server.printPayload((byte*)senPtr,sizeof(payload_t));
  ramQUpdateHead();
}

