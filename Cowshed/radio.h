#ifndef _RADIO_H_
#define _RADIO_H_
#include "IoT.h"

void radio_begin(void);
void radioStart();
uint32_t getRtcTime();

void eepromRead(uint32_t addr, uint8_t *buf, uint16_t len);
void eepromUpdate(uint32_t addr, uint8_t *buf, uint16_t len);


extern nrfNodeConfig_t nrfConfig;

#endif
