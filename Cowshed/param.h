#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>

/**********************************Common Build Flags************************/
//#define FACTORY_RESET
//#define DATA_ERASE
#define DEVICE_HAS_FLASH_MEMORY //if device has flash memory. 
#define DEVICE_HAS_RTC
//#define DO_CALIBRATION
#define  PCB_V_0_1_0
//#define  PCB_V_0_2_0

/******************Device Identifier********************/
#define SENSOR_TYPE 2
#define SENSOR_ID   81

/******************MEMORY and Buffer Param*********************/
#define TOTAL_PAYLOAD_BUFFER 2
#define MEMQ_RING_BUF_LEN  4
#define MEMQ_FLASH_START_ADDR 0
#define MEMQ_TOTAL_BUFFER   256
#define MEMQ_SECTOR_ERASE_SZ  4096
#define MEMQ_PTR_SAVE_AFTER   10  

/****************Interval Parameter********************/
#define DATA_ACQUIRE_INTERVAL   1
#define DATA_TRASNFER_INTERVAL  6


/***************Communication Parameter****************/
#define SPI_SPEED 1000000UL
#define SERIAL_SPEED  250000UL
/****************EEPROM MEMORY MAP*********************/
#define MQ4_EEP_ADDR         0
#define MQ135_EEPROM_ADDR   (MQ4_EEP_ADDR + 21)
#define ROM_ADDR_FOR_TXD    (MQ135_EEPROM_ADDR +21)
#define CONFIG_EEPROM_ADDR  (ROM_ADDR_FOR_TXD+ 20)
#define RING_EEPROM_ADDR    (CONFIG_EEPROM_ADDR+ 10)


/***************RING BUFFER PARAM**********************/


#endif
