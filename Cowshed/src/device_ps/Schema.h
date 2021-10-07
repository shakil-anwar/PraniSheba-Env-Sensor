#ifndef _DATA_SCHEMA_H_
#define _DATA_SCHEMA_H_
#include <Arduino.h>
#include "device.h"

struct dummy_t
{
  uint8_t temp[32];
};

// typedef struct struct sensor_t
// {
//   uint8_t type;
//   uint8_t reserve;
  
//   uint16_t id;
//   uint32_t unixTime;
//   float ammonia;
//   float methane;
//   float hum;
//   float temp;
// };

struct header_t
{
  uint8_t type;
  uint8_t checksum;
  uint16_t id;
};

struct sensor_t
{
  //header
  struct header_t header;
  //payload
  uint32_t unixTime;
  float ammonia;
  float methane;
  float hum;
  float temp;
  uint16_t logicVolt;
  uint16_t errorCode;
  uint8_t isServerSynced;
  uint8_t reserve[3];
};
struct sensorTh_t
{
  float sensorTh[4];
  uint16_t thresholdCount[4];
  uint16_t count;
  bool flag[4];
};

struct gasSensorLog_t
{
  struct header_t header;
  uint8_t errorCode;
  uint8_t hardwareErrorCode;
  uint16_t restartCount;
  uint16_t slotMissed;
  uint16_t samplingFreq;
  float railVoltage;
  uint32_t unixTime;
  uint32_t flashAvailablePackets;
};

struct gasSensorMetaLog_t
{
  uint16_t restartCount;
  uint32_t lastUnixTime;
  uint8_t reserve;
  uint8_t checksum;
};

typedef union payload_t
{
  struct sensor_t gasSensor;
  struct dummy_t dummy; 
};
/*************************Query Schema*****************************/


void schemaBegin();
struct sensor_t *getSensorsData(struct sensor_t *senPtr);
void printSensor(struct sensor_t *sensor);

void schemaReadSensors();
void memqSave();


void setSensorTh(float am, float mt, float hum, float temp, uint16_t num);
bool checkSensorth();

extern volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
// extern queryData_t queryBuffer;
extern struct sensor_t sensor;
extern sensorTh_t sensorTh;

#endif 
