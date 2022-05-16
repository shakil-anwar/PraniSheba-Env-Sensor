#ifndef _PIN_H_
#define _PIN_H_
#include  "../IoT.h"
/*******************************************************
 *                      PIN Layout
 *******************************************************/



/*****************NRF24L01 PINOUT*****************************/
/*Arduino pinout
#define NRF_CE    10
#define NRF_CSN   9
#define NRF_IRQ   3
*/
#if defined(ESP32_V010)
#if  defined(SOFTWARE_I2C)
	#warning <---------------------ESP32 Software I2C--------------------------->
	/**********************NRF24L01 PINOUT************/
	#define NRF_CE_PIN 14
	#define NRF_CSN_PIN 12
	#define NRF_IRQ 15
	/**************** FLASH PINOUT ******************/
	#define FLASH_CS 26
	#define FLASH_HOLD 2
	/**********Shift Resigter PIN diagram**************/
	// #define TOTAL_SR 42
	// #define SR_DATA 43
	// #define SR_CLOCK 45
	// #define SR_LATCH 46
	// #define SR_PWM 41//random
	/***************MQ Pin diagram*******************/
	#define MQ4_PIN 36
	#define MQ135_PIN 39
	/**************Buzzer pin*********/

	#define BUZZER_PIN 100 //25
	#define CONFIG_BTN_PIN 13

	#define BUZZER_OUT_MODE() pinMode(BUZZER_PIN,OUTPUT)
	#define BUZZER_ON() digitalWrite(BUZZER_PIN, HIGH)
	#define BUZZER_OFF() digitalWrite(BUZZER_PIN, LOW);

	/***************SHT21 Pin diagram*******************/
	//software spi pin
	#define SHT21_SDA 21 
	#define SHT21_SCL 22
	/***************DS1302 Pin diagram*******************/
	#define DS1302_IO 21
	#define DS1302_SCLK 22
	#define DS1302_CE 5
#else
		#warning <---------------------ESP32--------------------------->
	/**********************NRF24L01 PINOUT************/
	#define NRF_CE_PIN 		16
	#define NRF_CSN_PIN 	17
	#define NRF_IRQ 		13
	/**************** FLASH PINOUT ******************/
#if defined(GSM_V010)
	#define FLASH_CS 		14		//prev =  	16
	#define FLASH_HOLD 		5		//prev = 	17
#else
	#define FLASH_CS 		16
	#define FLASH_HOLD 		17
#endif
	/**********Shift Resigter PIN diagram**************/
	#define TOTAL_SR 		2
	#define SR_DATA 		4
	#define SR_CLOCK 		15
	#define SR_LATCH 		2

	// #define SR_PWM 41//random
	/***************MQ Pin diagram*******************/
	#define MQ4_PIN 		36
	#define MQ135_PIN 		39
	#define MQ2_PIN			34
	#define VDD_PIN			35
	/**************Buzzer pin*********/

	#define BUZZER_PIN 		26
	#define CONFIG_BTN_PIN 	25

	#define BUZZER_OUT_MODE() 	pinMode(BUZZER_PIN,OUTPUT)
	#define BUZZER_ON() 		digitalWrite(BUZZER_PIN, HIGH)
	#define BUZZER_OFF() 		digitalWrite(BUZZER_PIN, LOW);

	#define WDT_PIN			27
#endif
	

#elif defined(PCB_V_0_1_0)
	#define NRF_CE_PORT   PORTB
	#define NRF_CE_PIN    PB2 
	#define NRF_CSN_PORT  PORTB
	#define NRF_CSN_PIN   PB1
	#define NRF_IRQ   	  3
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

	#define CONFIG_BTN_PIN	2

	#define BUZZER_OUT_MODE() delay(1)
	#define BUZZER_ON()		  delay(1)
	#define BUZZER_OFF()	  delay(1)

#elif defined(PCB_V_0_2_0)
	#define NRF_CE_PORT   PORTB
	#define NRF_CE_PIN    PB1
	#define NRF_CSN_PORT  PORTB
	#define NRF_CSN_PIN   PB0
	#define NRF_IRQ   	  2

	#define FLASH_CS 	  6
	#define FLASH_HOLD	  7

	#define TOTAL_SR  3
	#define SR_DATA   5
	#define SR_CLOCK  4
	#define SR_LATCH  3
	#define SR_PWM    10
	
	#define MQ4_PIN   A0
	#define MQ135_PIN A1

	#define CONFIG_BTN_PIN	A2
	
	#define BUZZER_OUT_MODE() (DDRC |= 1<<PC3)
	#define BUZZER_ON()		  (PORTC |= 1<<PC3)
	#define BUZZER_OFF()	  (PORTC &= ~(1<<PC3))
#else
	#error "No Board Selected"
#endif

#endif
