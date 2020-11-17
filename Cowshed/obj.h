#ifndef _OBJECTS_H_
#define _OBJECTS_H_
#include <Arduino.h>
//#include "serial_pipe.h"
#include "MemQ.h"
#include "asyncServer.h"

void objectsBegin();


extern Flash flash;       //CS PIN = 5
extern RingEEPROM myeepRom;
extern MemQ memQ;
extern AsyncServer server;
#endif
