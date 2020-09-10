#ifndef _MQ_H_
#define _MQ_H_
#include "Arduino.h"

class MQ
{
  public:
    MQ(byte channel);
    void setGraphPoints(float x1, float x2, float y1, float y2);
    void setRl(float RL);
    float getPPM();
    float getRs();
    float calculateR0(float air_Rs_by_R0);
    float Rs;
  private:
    float _calculateRs();
    byte _analogPin;
    float _m;
    float _c;
    float _RL;
    float _R0;
};
#endif
