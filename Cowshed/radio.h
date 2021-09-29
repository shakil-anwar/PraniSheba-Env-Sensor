#ifndef _RADIO_H_
#define _RADIO_H_
#include "IoT.h"

void radio_begin(void);
void radioStart();
uint32_t getRtcTime();




extern nrfNodeConfig_t nrfConfig;

#endif
