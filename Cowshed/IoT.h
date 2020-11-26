#ifndef _OBJECTS_H_
#define _OBJECTS_H_
#include <Arduino.h>
#include <SoftwareSerial.h>

/**********Used Library*************/
#include "MemQ.h"
#include "asyncServer.h"
#include "nRF24.h"
#include "nRF24_Query.h"
#include "tScheduler.h"
#include "watchdog.h"
#include "realTime.h"
#include "ramQ.h"
#include "MQ.h"

/*********Common Library for all files**********/
#include "param.h"

void objectsBegin();


extern Flash flash;       //CS PIN = 5
extern RingEEPROM myeepRom;
extern MemQ memQ;
extern AsyncServer server;
#endif
