#ifndef _ALL_GLOBAL_H
#define _ALL_GLOBAL_H
#include "IoT.h"
#include "radio.h"


typedef enum mainState_t
{
  CHECK_HARDWARE,
  START_DEVICE,
  SYNCHRONIZE,
  DEVICE_RUN,
  STOP
};

void system_setup(void);
void test_flash(void);
void payloadStateMachine();
void dataSendStateMachine();


bool setDeviceConf();
bool syncTime();
bool isHardwareOk();
void startDevice();
void deviceRunSM();



#endif 
