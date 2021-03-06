#ifndef _TIMER1_H
#define _TIMER1_H
#if !defined(ARDUINO_ARCH_ESP32)
#include <Arduino.h>

#define RESOLUTION 65536    // Timer1 is 16 bit

class Timer1
{
  public:
    void initialize(float freq);
    void setTopValue(float freq);
    //    void attachInterrupt(void (*isr)());
    void attachIntCompB(void (*isr)() = NULL);
    //    void detachInterrupt();
    // uint32_t ms()
    void start();
    void stop();
    // virtual void isrCallback() = 0;
    void (*isrCallback)(); //Function pointer for interrupt routine
  private:
    unsigned char _prescalerBits;
    // uint32_t _freq = 0;
};

extern Timer1 timer1;
#endif
#endif
