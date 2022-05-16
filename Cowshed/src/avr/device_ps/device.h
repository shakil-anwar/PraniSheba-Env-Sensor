#ifndef _DEVICE_H_
#define _DEVICE_H_
// #include  "../../IoT.h"
#include  "../IoT.h"

#include "pin.h"
#include "Schema.h"
#include "Sensors.h"
// #include "Payload.h"
#include "led.h"
#include "./conf/confSimple.h"

void deviceBegin();
uint8_t *deviceMemRead();
#if defined(STAND_ALONE)
bool deviceRfSend(uint8_t *data);
#else
void deviceRfSend(uint8_t *data);
#endif
int deviceRfAckWait();
// void updateDataInterval(uint32_t time);

// uint8_t *readMem();
// void sendNrf(uint8_t *data);
// int ackWait();
uint16_t calculateLen(char *start, char *end);
bool isFlashRunning();


#endif