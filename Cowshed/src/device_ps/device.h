#ifndef _DEVICE_H_
#define _DEVICE_H_
#include  "../../IoT.h"
#include "pin.h"
#include "Schema.h"
#include "Payload.h"
#include "led.h"

uint8_t *readMem();
void sendNrf(uint8_t *data);
int ackWait();

void deviceBegin();


#endif