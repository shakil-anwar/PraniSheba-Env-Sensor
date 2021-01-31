/*
#if defined(ARDUINO_ARCH_AVR)
#include "Serial_ard.h"
#include "HardwareSerial.h"

cSerial_t *SerialPtr = NULL;

extern "C" {
  cSerial_t  *cMySerial()
  {
    return &Serial;
  }
  void //serial_begin(void)
  {
    SerialPtr = cMySerial();
//    SerialPtr -> begin(9600);
  }
  void //serial_print(char *str)
  {
    SerialPtr -> print(str);
  }
  void //serial_print_uint8(uint8_t n)
  {
    SerialPtr -> print(n);
  }
  void //serial_debug_print_uint8(uint8_t n,int8_t base)
  {
    SerialPtr -> print(n,base);
  }
}

#endif

*/