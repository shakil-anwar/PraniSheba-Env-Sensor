#include "utility.h"

<<<<<<< HEAD
=======

>>>>>>> update_june
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


void eepromRead(uint32_t addr, uint8_t *buf, uint16_t len)
{

  uint16_t eepAddr = (uint16_t)addr;
  uint8_t *ptr = buf;
  Serial.print(F("EEPROM>R>: ")); Serial.println(eepAddr);
  for (uint16_t i = 0 ; i < len; i++)
  {
    *(ptr + i) = EEPROM.read(eepAddr + i);
    // Serial.print( *(ptr + i)); Serial.print(F("  "));
  }
  // Serial.println();
}

void eepromUpdate(uint32_t addr, uint8_t *buf, uint16_t len)
{

  uint16_t eepAddr = (uint16_t)addr;
  uint8_t *ptr = buf;
  Serial.print(F("EEPROM>W>: ")); Serial.println(eepAddr);
  for (uint16_t i = 0; i < len; i++)
  {
    EEPROM.update(eepAddr + i, *(ptr + i));
    // Serial.print( *(ptr + i)); Serial.print(F("  "));
  }
  // Serial.println();
}


