#ifndef _OBJECTS_H_
#define _OBJECTS_H_
#include <Arduino.h>
#include <SoftwareSerial.h>

/**********Used Library*************/
#include "MemQ.h"
#include "asyncXfer.h"
#include "nRF24.h"
#include "nRF24_Query.h"
#include "tScheduler.h"
#include "watchdog.h"
#include "realTime.h"
#include "ramQ.h"
#include "MQ.h"
#include "FlashMemory.h"
#include "RingEEPROM.h"
#include "AVR_Timer1.h"


/*********Third Party Library******************/
#include <ShiftRegister74HC595.h>
#include <SHT21.h>
#include "RTClib.h"
/*********Common Library for all files**********/
#include "param.h"
#include "utility.h"
#include "./src/device_ps/device.h"
//#include "./src/device1/device.h"


void objectsBegin();

void rtcBegin();
uint32_t rtcGetSec();
void rtcUpdateSec(uint32_t unix);


extern Flash flash;       
extern RingEEPROM myeepRom;
extern MemQ memQ;
extern Scheduler scheduler;
//extern AsyncServer server;
#endif
