#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>

#define DEV //development flag
#define DEVICE_HAS_FLASH_MEMORY //if device has flash memory. 
//#define DO_CALIBRATION
//#define FACTORY_RESET

/******************Device Identifier********************/
#define SENSOR_TYPE 2
#define SENSOR_ID   81

/******************Menory and Buffer Param*********************/
#define TOTAL_PAYLOAD_BUFFER 10


/****************Interval Parameter********************/
#define DATA_ACQUIRE_INTERVAL  5
#define DATA_TRASNFER_INTERVAL 30

/****************EEPROM MEMORY MAP*********************/
#define MQ4_EEP_ADDR         0
#define MQ135_EEPROM_ADDR   (MQ4_EEP_ADDR + 21)
#define ROM_ADDR_FOR_TXD    (MQ135_EEPROM_ADDR +21)
#define RING_EEPROM_ADDR    (ROM_ADDR_FOR_TXD+ 20)


#endif
