#ifndef _DEVICE_H_
#define _DEVICE_H_
#include  "../../IoT.h"
#include "pin.h"
#include "Schema.h"
#include "Sensors.h"
#include "Payload.h"
#include "led.h"
#include "confSimple.h"

void deviceBegin();
uint8_t *deviceMemRead();
void deviceRfSend(uint8_t *data);
int deviceRfAckWait();

// uint8_t *readMem();
// void sendNrf(uint8_t *data);
// int ackWait();


#endif