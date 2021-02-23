#ifndef _SERIAL_STR_H_
#define _SERIAL_STR_H_
#include "Arduino.h"

uint8_t getSerialCmd();
int timedRead();
char *stringRead(char *buffer);
char *getSerialString(char *str);

#endif
