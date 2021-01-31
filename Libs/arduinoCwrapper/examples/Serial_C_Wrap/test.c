#include "test.h"
#include "Serial.h"

void testBegin()
{
  SerialBegin(9600);
}

void testPrint()
{
  uint8_t val = 34;
  SerialPrintlnP(F("Value is : "));SerialPrintlnU8(val);
}
