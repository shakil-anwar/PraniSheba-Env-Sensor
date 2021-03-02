#ifndef _MQ_H_
#define _MQ_H_
#include "Arduino.h"


typedef struct calib_t
{
  float c;
  float m;
  float RL;
  float R0;
  float airRsR0;
  bool  done;
};

// void saveCalib(calib_t *cPtr);
// void readCalib(calib_t *cPtr);

typedef void (*funCalib_t)(calib_t*);

class MQ
{
  public:
    MQ(byte channel);
    // MQ(byte ch, float RL, float airRsR0);
    // void setMemFun(funCalib_t save, funCalib_t read);

    void setXY(float x1, float x2, float y1, float y2);
    void setR(float RL, float air_Rs_by_R0);
    void runCalib(funCalib_t save);

    void beginFromMem(funCalib_t read);

    float getPPM(); 
    void printCalib(calib_t *cPtr);
  private:
    float _calculateRs();
    float calculateR0(float air_Rs_by_R0);
    byte  _analogPin;
    calib_t  _calib;
    // funCalib_t  _saveCalib = NULL;
    // funCalib_t  _readCalib = NULL;
};
#endif
