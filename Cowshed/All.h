#ifndef _ALL_GLOBAL_H
#define _ALL_GLOBAL_H
#include "IoT.h"
#include "radio.h"


typedef enum mainState_t
{
  CHECK_HARDWARE,
  START_DEVICE,
  SYNC_DEVICE,
  SYNC_RF,
  RUN_LOOP,
  STOP
};

typedef enum runState_t
{
  RUN_WAIT,
  RUN_CHK_BS_CONN,
  RUN_TX_XFER,
  RUN_END_TRANSFER,
};

enum tdmSyncState_t
{
  TDM_SYNCED,
  TDM_UNSYNCED,
  TDM_SLOT_MISSED,
  TDM_CONFIG_CHANGED,
  TDM_CONFIGURED,
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
void bsSendSm();



void configSave(config_t *bootPtr);
void configRead(config_t *bootPtr);


extern mainState_t mainState;
extern volatile uint32_t _nowSec;
extern volatile uint32_t _prevRunSec;
extern uint32_t _nextSlotUnix;
extern int16_t rfFailCount;
extern enum tdmSyncState_t tdmSyncState;

#endif 
