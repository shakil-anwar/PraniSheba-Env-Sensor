#ifndef _OBJECTS_H_
#define _OBJECTS_H_
#include <Arduino.h>
#if !defined(ARDUINO_ARCH_ESP32)
#include <SoftwareSerial.h>
#endif
#include "param.h"
#include "Timers.h"

#if defined(PROD_BUILD)    
    #include "./lib/memq/memq.h"
    #include "./lib/asyncXfer/asyncXfer.h"
    #include "./lib/nRF24/nRF24.h"
    #include "./lib/nRF24/nRF24_Client.h"
    // #include "./lib/tScheduler/tScheduler.h"
    #if !defined(ESP32_V010)
        #include "./lib/AVR_Watchdog/watchdog.h"
        #include "./lib/Timer1/src/AVR_Timer1.h"
    #endif

    #include "./lib/realTime/src/realTime.h"
    #include "./lib/ramQ/src/ramQ.h"
    #include "./lib/MQSensor/src/MQ.h"
    #include "./lib/FlashMemory/src/FlashMemory.h"
    #include "./lib/RingEEPROM/src/RingEEPROM.h"
    #include "../common_lib/iotStructure/iotStructure.h"
    
    

    // #include "./lib/RTClib/RTClib.h"
   
    #if defined(ESP32_V010)
        #include "../common_lib/commonTask/commonTask.h"
        #include "./lib/simple-json/simple_json.h"
        #include "../common_lib/ringQ/ringQ.h" 
        #include "../common_lib/FM24I2C/FM24I2C.h"

    #endif

    #if defined(ESP32_V010) && defined(SOFTWARE_I2C)
        #include "./lib/RTC_1302/src/ThreeWire.h"
        #include "./lib/RTC_1302/src/RtcDS1302.h"
    #else
        #include "./lib/Time/TimeLib.h"
        #include "./lib/ErriezDS1307/ErriezDS1307.h"
    #endif

    #include "./lib/ShiftRegister74HC595/src/ShiftRegister74HC595.h"
    #include "./lib/SHT21-Arduino-Library/SHT21.h"
#else
//    #include "MemQ.h"
    #include "memq.h"
    #include "asyncXfer.h"
    #include "nRF24.h"
    #include "nRF24_Client.h"
    
    #if !defined(ARDUINO_ARCH_ESP32)
        #include "watchdog.h"
        // #include "AVR_Timer1.h"
        // #include "tScheduler.h"
    #endif
    #include "realTime.h"
    #include "ramQ.h"
    #include "MQ.h"
    #include "FlashMemory.h"
    #include "RingEEPROM.h"
    

    // #include "RTClib.h"
    #include <TimeLib.h>
    #include <DS1307RTC.h>
    #include <ShiftRegister74HC595.h>
    #include <SHT21.h>
#endif

/*********Third Party Library******************/
// #include <ShiftRegister74HC595.h>
// #include <SHT21.h>
// #include "RTClib.h"
/*********Common Library for all files**********/

#include "utility.h"
#include "./device_ps/device.h"


uint32_t millis_esp32();

void objectsBegin();
void gpioBegin();
// void rtcBegin();
// uint32_t rtcGetSec();
// void rtcUpdateSec(uint32_t unix);
void factoryReset();




#if defined(DEVICE_HAS_LOG)
void initiateLog();
struct gasSensorLog_t *saveLog();
extern struct gasSensorLog_t sensorLog;
void resetLog(void);
#endif


extern Flash flash;       
//extern RingEEPROM myeepRom;
//extern MemQ memQ;
extern struct memq_t memq;
// extern Scheduler scheduler;

//extern AsyncServer server;
#endif
