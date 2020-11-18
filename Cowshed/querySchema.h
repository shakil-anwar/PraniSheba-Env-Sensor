#ifndef _QUERY_SCHEMA_H_
#define _QUERY_SCHEMA_H_
#include <Arduino.h>

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

typedef enum query_state_t
{
  QUERY_READ,
  QUERY_RESOLVE,
  QUERY_REPLY,
  QUERY_WAIT,
  QUERY_SUCCESS,
  QUERY_FAIL,
  QUERY_COMPLETE
};

extern queryData_t queryBuffer;
#endif
