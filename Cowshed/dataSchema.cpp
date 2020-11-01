#include "dataSchema.h"

sensor_t sensor; //global variable for sensor data handling.

void dataSchemaBegin()
{
  //Initialise sensor constant parameter
  sensor.type  = SENSOR_TYPE;
  sensor.id = COWSHED_ID;
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
  Serial.print(F("Temperature : "));Serial.print(sensor -> temp );Serial.println(" C");
  Serial.print(F("Humidity : "));Serial.print(sensor -> hum );Serial.println(" %Rh");
  Serial.print(F("Ammonia : "));Serial.print(sensor -> ammonia );Serial.println(" ppm");
  Serial.print(F("Methane : "));Serial.print(sensor -> methane );Serial.println(" ppm");
}
