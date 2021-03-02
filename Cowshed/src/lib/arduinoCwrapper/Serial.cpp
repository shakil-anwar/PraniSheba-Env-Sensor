#include "Serial.h"
#include "HardwareSerial.h"

Serial_t *SerialPtr = NULL;

extern "C" {
  Serial_t  *newMySerial()
  {
    return &Serial;
  }

  void SerialBegin(uint32_t baud)
  {
    SerialPtr = newMySerial();
    // SerialPtr -> begin(baud);
  }
  void SerialPrintF(const char *str)
  {
    SerialPtr -> print((__FlashStringHelper*)str);
  }
  void SerialPrintlnF(const char *str)
  {
    SerialPtr -> println((__FlashStringHelper*)str);
  }
  void SerialPrint(char *str)
  {
    SerialPtr -> print(str);
  }
  void SerialPrintln(char *str)
  {
    SerialPtr -> println(str);
  }
  void SerialPrintU8(uint8_t n)
  {
    SerialPtr -> print(n);
  }
  void SerialPrintS8(int8_t n)
  {
    SerialPtr -> print(n);
  }
  void SerialPrintU16(uint16_t n)
  {
    SerialPtr -> print(n);
  }
  void SerialPrintS16(int16_t n)
  {
    SerialPtr -> print(n);
  }
  void SerialPrintU32(uint32_t n)
  {
    SerialPtr -> print(n);
  }
  void SerialPrintS32(int32_t n)
  {
    SerialPtr -> print(n);
  }


  void SerialPrintlnU8(uint8_t n)
  {
    SerialPtr -> println(n);
  }
  void SerialPrintlnS8(int8_t n)
  {
    SerialPtr -> println(n);
  }
  void SerialPrintlnU16(uint16_t n)
  {
    SerialPtr -> println(n);
  }
  void SerialPrintlnS16(int16_t n)
  {
    SerialPtr -> println(n);
  }
  void SerialPrintlnU32(uint32_t n)
  {
    SerialPtr -> println(n);
  }
  void SerialPrintlnS32(int32_t n)
  {
    SerialPtr -> println(n);
  }
  void SerialPrintFloat(float n, uint8_t digit)
  {
    SerialPtr -> print(n, digit);
  }
  void SerialPrintlnFloat(float n, uint8_t digit)
  {
    SerialPtr -> println(n, digit);
  }
}
