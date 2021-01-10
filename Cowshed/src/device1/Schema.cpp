#include "Schema.h"
#include "Sensors.h"

volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
queryData_t queryBuffer;

// sensor_t sensor;

void schemaBegin()
{
  /**************Compile time checking************************/
  static_assert(!(TOTAL_PAYLOAD_BUFFER % 2), "\"TOTAL_FLASH_BUFFERS\" Should be Even Number");

  ramQSet(&payload, sizeof(payload_t), TOTAL_PAYLOAD_BUFFER);
  //Initialise sensor constant parameter
//  sensor.type  = SENSOR_TYPE;
//  sensor.id = COWSHED_ID;
}


sensor_t *getSensorsData(sensor_t *senPtr)
{
  sensor_t *sensor = senPtr;

  sensor -> type = SENSOR_TYPE;
  sensor -> id = SENSOR_ID;
  sensor -> unixTime = second();
  sensor -> voltage = getVoltage();
  sensor -> current = getCurrent();
  sensor -> power = (sensor -> voltage)*sensor -> current;
  sensor -> temp = getTemp();
  return senPtr;
}

void printSensor(sensor_t *sensor)
{
  Serial.println(F("<---------Sensor Data----------------->"));
  Serial.print(F("Time : ")); Serial.println(sensor -> unixTime );
  Serial.print(F("ID : ")); Serial.println(sensor -> id );
  Serial.print(F("Voltage : ")); Serial.print(sensor -> voltage); Serial.println(" V");
  Serial.print(F("Current : ")); Serial.print(sensor -> current ); Serial.println(" A");
  Serial.print(F("Power : ")); Serial.print(sensor -> power); Serial.println(" W");
  Serial.print(F("Temperature : ")); Serial.print(sensor -> temp); Serial.println(" °C");
}


void schemaReadSensors()
{
  sensor_t *senPtr = (sensor_t*)ramQHead();
  getSensorsData(senPtr);
  printSensor(senPtr);
  Serial.print(F("ramQ Counter :"));Serial.println(ramQCounter);
  
//  nrfSend((const uint8_t*)senPtr,sizeof(payload_t));
//  server.printPayload((byte*)senPtr,sizeof(payload_t));
  ramQUpdateHead();
}
