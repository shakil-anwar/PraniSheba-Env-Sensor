#ifndef _ALL_GLOBAL_H
#define _ALL_GLOBAL_H
#include "IoT.h"
#include "radio.h"


typedef enum mainState_t
{
  CHECK_HARDWARE,
  START_DEVICE,
  SYNC_DEVICE,
  DEVICE_RUN,
  STOP
};

//typedef enum runState_t
//{
//  
//};

void system_setup(void);
void test_flash(void);
void payloadStateMachine();
void dataSendStateMachine();


bool setDeviceConf();
bool syncTime();
bool isHardwareOk();
void startDevice();
void deviceRunSM();

void readAddr(addr_t *addrPtr);
void saveAddr(addr_t *addrPtr);

#endif 
