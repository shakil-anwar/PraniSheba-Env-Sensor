#ifndef _RADIO_H_
#define _RADIO_H_
#include "IoT.h"

void radio_begin(void);
void radioStart();
uint32_t getRtcTime();

void readAddr(addr_t *addrPtr);
void saveAddr(addr_t *addrPtr);

#endif
