#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>

#define DEV //development flag
/*********************************************************
 *                     Device Parameter
 ********************************************************/

/******************Device Identifier********************/
#define SENSOR_TYPE 2
#define COWSHED_ID 80

/******************Buffer Parameters*********************/
#define TOTAL_PAYLOAD_BUFFER 2

/*****************Gas Sensor Parameter******************/
#define MQ4_AIR_RS_R0_RATIO 1.0f
#define MQ135_AIR_RS_R0_RATIO 3.7f

/****************Interval Parameter********************/
#define DATA_ACQUIRE_INTERVAL  2






/*******************************************************
 *                      PIN Layout
 *******************************************************/

 
/*****************NRF24L01 PINOUT*****************************/
#define NRF_CE    10
#define NRF_CSN   9
#define NRF_IRQ   3
/**************** FLASH PINOUT ******************/
#define FLASH_CS  8

///Shift Resigter PIN diagram 
#define TOTAL_SR  3
#define SR_DATA   7
#define SR_CLOCK  6
#define SR_LATCH  5
#define SR_PWM    4

///MQ Pin diagram 
#define MQ4_PIN   A0
#define MQ135_PIN A1




#endif
