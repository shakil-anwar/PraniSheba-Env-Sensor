#ifndef _ALL_GLOBAL_H
#define _ALL_GLOBAL_H
#include <Arduino.h>
#include "tScheduler.h"
#include "led.h"
#include "pin.h"
#include "radio.h"
#include "ToServer.h"

void system_setup(void);
void test_flash(void);
void payloadStateMachine();
void dataSendStateMachine();


extern Scheduler scheduler;
#endif 
