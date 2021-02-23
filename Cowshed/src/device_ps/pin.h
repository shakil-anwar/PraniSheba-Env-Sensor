#ifndef _PIN_H_
#define _PIN_H_
#include  "../../IoT.h"
/*******************************************************
 *                      PIN Layout
 *******************************************************/



/*****************NRF24L01 PINOUT*****************************/
/*Arduino pinout
#define NRF_CE    10
#define NRF_CSN   9
#define NRF_IRQ   3
*/
#ifdef defined(PCB_V_0_1_0)
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
	#define SR_CLOCK  A6
	#define SR_LATCH  A7
	#define SR_PWM    10
	
	#define MQ4_PIN   A0
	#define MQ135_PIN A1

	#define CONFIG_BTN_PIN	A2
#else
	#error "No Board Selected"
#endif

#endif
