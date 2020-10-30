#ifndef _TIME_H_
#define _TIME_H_
#include <Arduino.h>


void timeBegin();
void rtcBegin();

uint32_t second();
void setSecond(uint32_t second);

uint32_t getUnixTime();
void updateTime(uint32_t unixTime);
void printDateTime(uint32_t unixTime);
#endif 
