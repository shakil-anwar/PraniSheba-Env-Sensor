#include "watchdog.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


uint16_t nextPowerOf2(uint16_t n);

static volatile wdtFun_t _cb;

ISR(WDT_vect)
{
  
  if (_cb)
  {
    // Serial.println(F("<Assigned>"));
    _cb();
  }
  else
  {
    Serial.println(F("<Task Not Assigned>"));
  }

  Serial.println(F("<wdt ISR>"));
  delay(1);
  MCUSR = 0;  //reset MCU status register
  WDTCSR |= (1 << WDCE) | (1 << WDE); // Enter config mode.
  WDTCSR  = (1 << WDE) | 0b000000; //Reset Enable
}

void wdtAttach(wdtFun_t cb)
{
  _cb = cb;
}

void wdtEnable(uint16_t timeMs)
{
  timeMs = nextPowerOf2(timeMs);
  uint8_t  reg = (uint8_t)(log (timeMs) / log (2) + 0.5) - 4; //logxn = logy n / logy x

  Serial.print(F("WDT Time(ms) :")); Serial.println(timeMs);
  uint8_t wdp3_bit = (reg >> 3) & 0x01;
  uint8_t wdp2_to_wdp0_bit = (reg & 0x07);

  char oldReg = SREG;
  cli();
  MCUSR = 0;
  WDTCSR |= (1 << WDCE) | (1 << WDE);       //Enter config mode.
  WDTCSR = (wdp3_bit << WDP3) | wdp2_to_wdp0_bit;
  SREG = oldReg;
  sei();
  _cb = NULL;
}

void wdtBegin(uint16_t timeMs, wdtFun_t wdtCb)
{
  timeMs = nextPowerOf2(timeMs);
  uint8_t  reg = (uint8_t)(log (timeMs) / log (2) + 0.5) - 4; //logxn = logy n / logy x

  Serial.print(F("WDT Time(ms) :")); Serial.println(timeMs);
  uint8_t wdp3_bit = (reg >> 3) & 0x01;
  uint8_t wdp2_to_wdp0_bit = (reg & 0x07);

  char oldReg = SREG;
  cli();
  MCUSR = 0;
  WDTCSR |= (1 << WDCE) | (1 << WDE);       //Enter config mode.
  WDTCSR = (wdp3_bit << WDP3) | wdp2_to_wdp0_bit;
  SREG = oldReg;
  sei();
  _cb = wdtCb;
}

void wdtStart()
{
  WDTCSR |= (1 << WDIE);
}

void wdtReset()
{
  wdt_reset();
}

void wdtDisable()
{
  MCUSR = 0;
  wdt_disable();
}

uint16_t nextPowerOf2(uint16_t n)
{
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  return n;
}
