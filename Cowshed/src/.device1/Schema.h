#ifndef _DATA_SCHEMA_H_
#define _DATA_SCHEMA_H_
#include <Arduino.h>
#include "device.h"


typedef struct sensor_t
{
  uint8_t type;     //device type
  uint8_t reserve;
  uint16_t id;        //deviceId
  uint32_t unixTime;
  float voltage;
  float current;
  float power;
  float temp;
  float lux;
  uint8_t checksum;  //might be used for checksum
};

typedef union payload_t
{
  sensor_t solarSensor; 
};
/*************************Query Schema*****************************/


typedef struct confPacket_t
{
  uint8_t type;
  uint8_t opCode;
  uint8_t txAddrByte;
  uint32_t uTime;
};


typedef struct unixTime_t
{
  uint8_t type;
  uint8_t opCode;
  uint32_t utime;
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
   uint32_t test;
};

typedef union queryData_t
{
  confPacket_t confPacket;
  unixTime_t unixTime;
  shedulePacket_t schedulePacket;
  bolus_config_t bolusConfig;
};

void schemaBegin();
void schemaReadSensors();

extern volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
extern queryData_t queryBuffer;
#endif 
