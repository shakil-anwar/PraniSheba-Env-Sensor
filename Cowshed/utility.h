#ifndef __UTILITY_H_
#define __UTILITY_H_
#include <Arduino.h>
#include "EEPROM.h"


extern bool deviceHasRtc;
float getRailVoltage();


void eepromRead(uint32_t addr, uint8_t *buf, uint16_t len);
void eepromUpdate(uint32_t addr, uint8_t *buf, uint16_t len);




#endif
