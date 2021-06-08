#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>

/**********************************Common Build Flags************************/
// #define  PCB_V_0_1_0
// #define  PCB_V_0_2_0

// #define FACTORY_RESET
// #define DATA_ERASE
#define DEVICE_HAS_FLASH_MEMORY //if device has flash memory. 
#define DEVICE_HAS_RTC
//#define DO_CALIBRATION
#define DEVICE_HAS_TDM




/******************Device Identifier********************/
#define SENSOR_TYPE 2
#define SENSOR_ID   81

/******************MEMORY and Buffer Param*********************/
#define TOTAL_PAYLOAD_BUFFER 2
#define MEMQ_RING_BUF_LEN  4
#define MEMQ_FLASH_START_ADDR 0
#define MEMQ_TOTAL_BUFFER   3600
#define MEMQ_SECTOR_ERASE_SZ  4096
#define MEMQ_PTR_SAVE_AFTER   10  

/****************Interval Parameter********************/
//#define DATA_ACQUIRE_INTERVAL   1
#define DATA_TRASNFER_INTERVAL  10
#define MAX_RF_DATA_SEND_RETRY  5

/***************Communication Parameter****************/
#if defined(PCB_V_0_1_0)
#define SPI_SPEED 1000000UL
#elif defined(PCB_V_0_2_0)
#define SPI_SPEED 10000UL
#endif

#define SERIAL_SPEED  250000UL
/****************EEPROM MEMORY MAP*********************/
#define MQ4_EEP_ADDR         0
#define MQ135_EEPROM_ADDR   (MQ4_EEP_ADDR + 21)
#define MAIN_CONFIG_EEPROM_ADDR  (MQ135_EEPROM_ADDR+ 21)
#define NRF_CONFIG_ROM_ADDR     (MAIN_CONFIG_EEPROM_ADDR+20)
#define RING_EEPROM_ADDR    (NRF_CONFIG_ROM_ADDR+ 20)


/***************RING BUFFER PARAM**********************/


#endif
