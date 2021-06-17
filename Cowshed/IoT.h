#ifndef _OBJECTS_H_
#define _OBJECTS_H_
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "param.h"
#include "Timers.h"

#if defined(PROD_BUILD)    
    #include "./src/lib/memq/memq.h"
    #include "./src/lib/asyncXfer/asyncXfer.h"
    #include "./src/lib/nRF24/nRF24.h"
    #include "./src/lib/nRF24/nRF24_Client.h"
    #include "./src/lib/tScheduler/tScheduler.h"
    #include "./src/lib/AVR_Watchdog/watchdog.h"
    #include "./src/lib/realTime/src/realTime.h"
    #include "./src/lib/ramQ/src/ramQ.h"
    #include "./src/lib/MQSensor/src/MQ.h"
    #include "./src/lib/FlashMemory/src/FlashMemory.h"
    #include "./src/lib/RingEEPROM/src/RingEEPROM.h"
    #include "./src/lib/Timer1/src/AVR_Timer1.h"

    #include "./src/lib/RTClib/RTClib.h"
    #include "./src/lib/ShiftRegister74HC595/src/ShiftRegister74HC595.h"
    #include "./src/lib/SHT21-Arduino-Library/SHT21.h"
#else
//    #include "MemQ.h"
    #include "memq.h"
    #include "asyncXfer.h"
    #include "nRF24.h"
    #include "nRF24_Client.h"
    #include "tScheduler.h"
    #include "watchdog.h"
    #include "realTime.h"
    #include "ramQ.h"
    #include "MQ.h"
    #include "FlashMemory.h"
    #include "RingEEPROM.h"
    #include "AVR_Timer1.h"

    #include "RTClib.h"
    #include <ShiftRegister74HC595.h>
    #include <SHT21.h>
#endif





/*********Third Party Library******************/
// #include <ShiftRegister74HC595.h>
// #include <SHT21.h>
// #include "RTClib.h"
/*********Common Library for all files**********/

#include "utility.h"
#include "./src/device_ps/device.h"
//#include "./src/device1/device.h"


void objectsBegin();
void gpioBegin();
// void rtcBegin();
// uint32_t rtcGetSec();
// void rtcUpdateSec(uint32_t unix);
void factoryReset();


extern Flash flash;       
//extern RingEEPROM myeepRom;
//extern MemQ memQ;
extern struct memq_t memq;
extern Scheduler scheduler;

#if defined(DEVICE_HAS_LOG)
void initiateLog();
struct gasSensorLog_t *updateLog();
extern struct gasSensorLog_t sensorLog;
void resetLog(void);
#endif
//extern AsyncServer server;
#endif
