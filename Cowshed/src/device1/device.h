#ifndef _DEVICE_H_
#define _DEVICE_H_
#include  "../../IoT.h"
#include "pin.h"
#include "Schema.h"
#include "Sensors.h"

// #define DEVICE_HAS_FLASH_MEMORY //if device has flash memory. 

uint8_t *readMem();
void sendNrf(uint8_t *data);
int ackWait();
void deviceBegin();


#endif