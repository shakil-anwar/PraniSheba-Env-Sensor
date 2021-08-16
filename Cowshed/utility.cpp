#include "utility.h"

#define REF_VOLTAGE 1.1




bool deviceHasRtc = true;


float getRailVoltage() {
  ADMUX |= (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1<< MUX1);
  ADMUX &= ~(1<<MUX0) & ~(1<<REFS1);
  delay(3); //delay for 3 milliseconds
  ADCSRA |= (1<< ADSC); // Start ADC conversion
  while (bit_is_set(ADCSRA,ADSC)); //wait until conversion is complete
  int result = ADCL; //get first half of result
  result |= ADCH<<8; //get rest of the result
  float railVolt = (REF_VOLTAGE / result)*1024; //Use the known REF_VOLTAGE to calculate rail voltage
  return railVolt;
}

