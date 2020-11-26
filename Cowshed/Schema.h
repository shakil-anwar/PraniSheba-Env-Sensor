#ifndef _DATA_SCHEMA_H_
#define _DATA_SCHEMA_H_
#include <Arduino.h>

/*********************Global Dataschema Parameters***************/
#define TOTAL_PAYLOAD_BUFFER 8

/*********************Gas Sensor Parameters**********************/
#define SENSOR_TYPE 2
#define COWSHED_ID 150


/************************Payload Schema**************************/
typedef struct sensor_t
{
  uint8_t type;
  uint8_t reserve;
  
  uint16_t id;
  uint32_t unixTime;
  float ammonia;
  float methane;
  float hum;
  float temp;
};

typedef union payload_t
{
  sensor_t gasSensor;
};
/*************************Query Schema*****************************/

typedef struct unixTime_t
{
  uint8_t type;
  uint8_t opCode;
  uint32_t time;
};

typedef struct shedulePacket_t
{
  uint8_t type; //device type
  uint8_t opCode;//device op code
  uint8_t slotNo;
  uint32_t unixTime;
  
};

typedef struct bolus_config_t
{
  uint8_t type; //device type
  uint8_t opCode;//device opcode
  uint16_t interval;
  uint32_t dataStartTime;
};

typedef union queryData_t
{
  unixTime_t unixTime;
  shedulePacket_t schedulePacket;
  bolus_config_t bolusConfig;
};

/******************************Public API**************************/





void dataSchemaBegin();
sensor_t *getSensorsData(sensor_t *senPtr);
void printSensor(sensor_t *sensor);

void mqBegin();
void mqCalibrate();
void humSensorBegin();

float getTemp();
float getHum();
float getAmmonia();
float getMethane();

void updateDisplay();

void dataAcquisition();

extern volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
extern queryData_t queryBuffer;

#endif 
