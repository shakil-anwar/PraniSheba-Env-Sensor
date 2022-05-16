#ifndef _AVR_H_
#define _AVR_H_
#include "All.h"


void avrSetup();
void avrloop();

void printMainState(mainState_t mstate);
bool syncTime();
bool rfConfig();

void setNRFconfig();


#endif //_AVR_H_