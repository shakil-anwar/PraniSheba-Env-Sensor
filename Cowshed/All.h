#ifndef _ALL_GLOBAL_H
#define _ALL_GLOBAL_H
#include "IoT.h"
#include "radio.h"


typedef enum mainState_t
{
  CHECK_HARDWARE,
  START_DEVICE,
  SYNC_DEVICE,
  RUN_LOOP,
  STOP
};

typedef enum runState_t
{
  RUN_WAIT,
  RUN_CHK_BS_CONN,
  RUN_TX_XFER,
  
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

void readAddr(addr_t *addrPtr);
void saveAddr(addr_t *addrPtr);

void configSave(config_t *bootPtr);
void configRead(config_t *bootPtr);

extern volatile uint32_t _nowSec;
extern volatile uint32_t _prevRunSec;

#endif 
