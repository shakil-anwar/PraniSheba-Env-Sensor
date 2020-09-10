#include "Arduino.h"


/*****************NRF24L01 PINOUT*****************************/
#define NRF_CE 
#define NRF_CSN 
#define NRF_IRQ 

///Shift Resigter PIN diagram 
#define TOTAL_SR  3
#define SR_DATA   2
#define SR_CLOCK  5
#define SR_LATCH  4
#define SR_PWM    6

///MQ Pin diagram 
#define MQ4_PIN   A0
#define MQ135_PIN A1

///NRF PIN Diagram 

///DHT Sensor 
#define DHTPIN 7     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
