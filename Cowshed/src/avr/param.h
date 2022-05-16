#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>
#include "./lib/memq/memq.h"
#include "../../conf.h"

/**********************************Common Build Flags************************/
// #define  PCB_V_0_1_0
// #define  PCB_V_0_2_0

// #define FACTORY_RESET
// #define DATA_ERASE
#define DEVICE_HAS_FLASH_MEMORY //if device has flash memory. 
#define DEVICE_HAS_RTC
#define DO_CALIBRATION      // Calibrate Gas Sensors at beginning
#define DEVICE_HAS_TDM
#define DEVICE_HAS_LOG

/*****************************Critical Checking*******************************/
// #define FLASH_HEALTH_CHECK       // This enable memq memory read write with random value to check flash condition.

// #define LED_TEST                 // This function test LEDs before starting a new device


#define JSON_BUFFER_SIZE 2048


/******************Device Identifier********************/
#define SENSOR_TYPE         11
#define SENSOR_LOG_TYPE     23
#define SENSOR_ID           RCMS_DEVICE_ID



/******************MEMORY and Buffer Param*********************/
#define TOTAL_PAYLOAD_BUFFER    2
#define MEMQ_RING_BUF_LEN       4
#define MEMQ_FLASH_START_ADDR   13000
#define MEMQ_TOTAL_BUFFER       3600
#define MEMQ_SECTOR_ERASE_SZ    4096
#define MEMQ_PTR_SAVE_AFTER     5

/****************Interval Parameter********************/
//#define DATA_ACQUIRE_INTERVAL   1
#define DATA_TRASNFER_INTERVAL      10
#define MAX_RF_DATA_SEND_RETRY      5

/***************Communication Parameter****************/
#if defined(PCB_V_0_1_0)
#define SPI_SPEED 1000000UL
#elif defined(PCB_V_0_2_0)
#define SPI_SPEED 10000UL
#elif defined(ESP32_V010)
#define SPI_SPEED 100000UL
#endif

#define SERIAL_SPEED  250000UL
/****************EEPROM MEMORY MAP*********************/
#define EEPROM_SIZE                 4096 //maxsize for esp32
#define MQ4_EEP_ADDR                0
#define MQ135_EEPROM_ADDR           (MQ4_EEP_ADDR + 21)
#define MAIN_CONFIG_EEPROM_ADDR     (MQ135_EEPROM_ADDR+ 21)
#define NRF_CONFIG_ROM_ADDR         (MAIN_CONFIG_EEPROM_ADDR+32)
#define RING_EEPROM_ADDR            (NRF_CONFIG_ROM_ADDR + 20)
#define LOG_SAVE_ADDR               (RING_EEPROM_ADDR +sizeof(struct memqPtr_t)*MEMQ_RING_BUF_LEN+MEMQ_RING_BUF_LEN*2+40)
#define SUBSCRIPTION_DATA_ADDR      (LOG_SAVE_ADDR + sizeof(struct gasSensorMetaLog_t)*MEMQ_RING_BUF_LEN+MEMQ_RING_BUF_LEN*2+10)           


/***************RING BUFFER PARAM**********************/


#endif
