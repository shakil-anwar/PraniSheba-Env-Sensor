#ifndef _TIMER1_H
#define _TIMER1_H
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
    void start();
    void stop();
    // virtual void isrCallback() = 0;
    void (*isrCallback)(); //Function pointer for interrupt routine
  private:
    unsigned char _prescalerBits;
};

extern Timer1 timer1;
#endif
