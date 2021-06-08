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
#if defined(PCB_V_0_1_0)
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
