#include "Timer1.h"

int adc_result = 0;

void setup() 
{
 Serial.begin(57600);
 Serial.println(F("Setup Done"));
 timer1.initialize(100); //1Hz timer
 timer1.attachIntCompB(timerIsr);
 timer1.start();
 adc.begin();
 adc.attachInterrupt(adcIsr);
 adc.setAutoTriggerSource(TIMER1_CMP_B);
 adc.startConversion(0);
 DDRB |= 1<<PB5; //Arduino default LED 13
 
}

void loop() 
{
 Serial.println(adc_result); 
}
void timerIsr()
{
//  PORTB ^= 1<<PB5;
}

void adcIsr()
{
  adc_result = ADCW;
  PORTB ^= 1<<PB5;
}

