#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>

#define DEV //development flag
#define DEVICE_HAS_FLASH_MEMORY //if device has flash memory. 

/******************Device Identifier********************/
#define SENSOR_TYPE 2
#define SENSOR_ID   81

/******************Menory and Buffer Param*********************/
#define TOTAL_PAYLOAD_BUFFER 10


/****************Interval Parameter********************/
#define DATA_ACQUIRE_INTERVAL  5

/*****************EEPROM Address Map*******************/
#define ROM_ADDR_FOR_TXD  0
#define RING_EEPROM_ADDR  20

#endif
