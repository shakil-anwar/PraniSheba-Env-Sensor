#include "dataSchema.h"

sensor_t sensor; //global variable for sensor data handling.

void dataSchemaBegin()
{
  //Initialise sensor constant parameter
  sensor.type  = SENSOR_TYPE;
  sensor.id = COWSHED_ID;
//  //initialize all sensors
//  humSensorBegin();
//  mqBegin();
//  mqCalibrate();
}

sensor_t *getSensorsData()
{
  sensor.temp = getTemp();
  sensor.hum = getHum();
  sensor.ammonia = getAmmonia();
  sensor.methane = getMethane();
  return &sensor;
}
void printSensor(sensor_t *sensor)
{
  Serial.println(F("<---------Sensor Data----------------->"));
  Serial.print(F("ID : "));Serial.println(sensor -> id );
  Serial.print(F("Temperature : "));Serial.println(sensor -> temp );
  Serial.print(F("Humidity : "));Serial.println(sensor -> hum );
  Serial.print(F("Ammonia : "));Serial.println(sensor -> ammonia );
  Serial.print(F("Methane : "));Serial.println(sensor -> methane );
}
