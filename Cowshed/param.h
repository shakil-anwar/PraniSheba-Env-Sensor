#ifndef _PARAM_H_
#define _PARAM_H_
#include <Arduino.h>

/*********************************************************
 *                     Device Parameter
 ********************************************************/
#define MQ4_AIR_RS_R0_RATIO 1.0f
#define MQ135_AIR_RS_R0_RATIO 3.7f


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

///NRF PIN Diagram 

///DHT Sensor 
#define DHTPIN A6           // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11       // DHT 11
//#define DHTTYPE DHT22     // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21     // DHT 21 (AM2301)


#endif
