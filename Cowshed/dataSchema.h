#ifndef _DATA_SCHEMA_H_
#define _DATA_SCHEMA_H_
#include <Arduino.h>

#define SENSOR_TYPE 2
#define COWSHED_ID 100

typedef struct sensor_t
{
  uint8_t type;
  uint16_t id;
  uint32_t unixTime;
  float ammonia;
  float methane;
  float hum;
  float temp;
};



void dataSchemaBegin();
sensor_t *getSensorsData();
void printSensor(sensor_t *sensor);

void mqBegin();
void mqCalibrate();
void humSensorBegin();

float getTemp();
float getHum();
float getAmmonia();
float getMethane();




#endif 
