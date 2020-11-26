#ifndef _ALL_GLOBAL_H
#define _ALL_GLOBAL_H
#include <Arduino.h>
#include "tScheduler.h"
#include "led.h"
#include "param.h"
#include "radio.h"
#include "watchdog.h"
#include "realTime.h"

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



bool isHardwareOk();
void startDevice();
void deviceRunSM();

extern Scheduler scheduler;
#endif 
