#ifndef _DATA_SCHEMA_H_
#define _DATA_SCHEMA_H_
#include <Arduino.h>
#include "device.h"
// #include "../param.h"
#define TOTAL_PAYLOAD_BUFFER 2


// struct dummy_t
// {
//   uint8_t temp[32];
// }__attribute__ ((packed));

// typedef struct struct gasSensor_t
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

// struct header_t
// {
//   uint8_t type;
//   uint8_t checksum;
//   uint16_t id;
// }__attribute__ ((packed));

// struct gasSensor_t
// {
//   //header
//   struct header_t header;
//   //payload
//   uint32_t unixTime;
//   float ammonia;
//   float methane;
//   float hum;
//   float temp;
//   uint16_t logicVolt;
//   uint16_t errorCode;
//   uint8_t isServerSynced;
//   uint8_t reserve[3];
// }__attribute__ ((packed));

// struct gasSensorLog_t
// {
//   struct header_t header;
//   uint8_t errorCode;
//   uint8_t hardwareErrorCode;
//   uint16_t restartCount;
//   uint16_t slotMissed;
//   uint16_t samplingFreq;
//   float railVoltage;
//   uint32_t unixTime;
//   uint32_t flashAvailablePackets;
// }__attribute__ ((packed));

// struct gasSensorMetaLog_t
// {
//   uint16_t restartCount;
//   uint32_t lastUnixTime;
//   uint8_t reserve;
//   uint8_t checksum;
// }__attribute__ ((packed));

// typedef union payload_t
// {
//   struct gasSensor_t gasSensor;
//   struct dummy_t dummy; 
// }payload_t;
/*************************Query Schema*****************************/


// typedef struct confPacket_t
// {
//   uint8_t type;
//   uint8_t opCode;
//   uint8_t txAddrByte;
//   uint32_t uTime;
// };


// typedef struct unixTime_t
// {
//   uint8_t type;
//   uint8_t opCode;
//   uint32_t utime;
// };

// typedef struct shedulePacket_t
// {
//   uint8_t type; //device type
//   uint8_t opCode;//device op code
//   uint8_t slotNo;
//   uint32_t unixTime;
  
// };

// typedef struct bolus_config_t
// {
//   uint8_t type; //device type
//   uint8_t opCode;//device opcode
//   uint16_t interval;
//   uint32_t dataStartTime;
//    uint32_t test;
// };

// typedef union queryData_t
// {
//   confPacket_t confPacket;
//   unixTime_t unixTime;
//   shedulePacket_t schedulePacket;
//   bolus_config_t bolusConfig;
// };


void schemaBegin();
struct gasSensor_t *getSensorsData(struct gasSensor_t *senPtr);
void printSensor(struct gasSensor_t *sensor);

void schemaReadSensors();
void memqSave();

void printPayload(payload_t *payload);
char *toJson( uint8_t *payloadP,char *buffer, uint8_t totalPayload = 1);
char *toJsonInit(char *buffer);
char *toJsonFinal(char *buffer);

extern volatile payload_t  payload[TOTAL_PAYLOAD_BUFFER];
// extern queryData_t queryBuffer;
extern struct gasSensor_t sensor;

#endif 
