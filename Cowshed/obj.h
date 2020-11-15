#ifndef _OBJECTS_H_
#define _OBJECTS_H_
#include <Arduino.h>
//#include "serial_pipe.h"
#include "MemQ.h"
#include "asyncServer.h"

void objectsBegin();
void pipeSendServer(const char *data);
char *toJson( uint8_t *payloadP, char *buffer, uint8_t totalPayload = 1);

//extern Pipe pipe;
extern Flash flash;       //CS PIN = 5
extern RingEEPROM myeepRom;
extern MemQ memQ;
extern AsyncServer server;
#endif
