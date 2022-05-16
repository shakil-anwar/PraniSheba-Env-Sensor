#ifndef __UTILITY_H_
#define __UTILITY_H_
#include <Arduino.h>
#include "EEPROM.h"


extern bool deviceHasRtc;
float getRailVoltage();
void printRailVoltage();
#if defined(ESP32_V010)
void eepromBegin(uint16_t eepromSize);
unsigned long majorityElement(unsigned long *values, int8_t dataLength);

#if defined(SUBCRIPTION_ON)
void loadSubcriptionPlan();
void saveSubscriptionPlan();
void initiateDummySucbscriptionData();
void printSubscriptionData(struct subscriptionParam_t *subParam);
#endif

// void espWdtBegin();
// void espWdtReset(bool bResetFlag = false);
#endif
void EEPROMRead(uint32_t addr, uint8_t *buf, uint16_t len);
void EEPROMUpdate(uint32_t addr, uint8_t *buf, uint16_t len);





#endif
