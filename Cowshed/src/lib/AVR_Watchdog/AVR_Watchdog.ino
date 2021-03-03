#include "watchdog.h"

void criticalTask();
void setup()
{
  Serial.begin(9600);
  wdtEnable(3000);
  wdtAttach(criticalTask);
  wdtStart();
  Serial.println(F("<-------Setup done------->"));
}

void loop()
{
  delay(1000);
  wdtReset();
//  wdt_reset();
}

void criticalTask()
{
  Serial.println(F("Critical Tasks"));
}

















//void setPrescaler(uint16_t timeMs)
//{
//
//  uint16_t n = nextPowerOf2(timeMs);
//  uint8_t  reg = (uint8_t)(log (n) / log (2) + 0.5) - 4; //logxn = logy n / logy x
//
//  Serial.println (reg, BIN);
//}
//uint16_t nextPowerOf2(uint16_t n)
//{
//  n--;
//  n |= n >> 1;
//  n |= n >> 2;
//  n |= n >> 4;
//  n |= n >> 8;
//  n |= n >> 16;
//  n++;
//  Serial.println(n);
//  return n;
//}
//
//void wdtBegin(uint16_t timeMs)
//{
//  timeMs = nextPowerOf2(timeMs);
//  uint8_t  reg = (uint8_t)(log (n) / log (2) + 0.5) - 4; //logxn = logy n / logy x
//
//  uint8_t wdp3_bit = (reg>>3) & 0x01;
//  uint8_t wdp2_to_wdp0_bit = (req & 0x07);
//
//  char oldReg = SREG;
//  cli();
//  MCUSR = 0;
//  WDTCSR |= (1<<WDCE)|(1<<WDE);             //Enter config mode.
//  WDTCSR = (1<<WDIE) | (wdp3_bit<<WDP3) | wdp2_to_wdp0_bit;
//  SREG = oldReg;
//  sei(); 
//}
