#ifndef _IOT_CLIENT_H_
#define _IOT_CLIENT_H_
#include <Arduino.h>
#include "sim800.h"


typedef enum //espIotState_t
{
  IOT_BEGIN,
  IOT_CONNECT,
  IOT_RUN_WIFI,
  IOT_RUN_GSM,
  IOT_IDLE,
} espIotState_t;


void iotClientBegin();

void wifiClientLoop();

void gsmClientLoop();

void iotClientLoopV2();
void iotConnect();
void espIoTClientLoop();

bool gprsConnStatus();

extern bool pingStatus;
extern espIotState_t espIotState;
extern bool startUpStatus;

#endif
