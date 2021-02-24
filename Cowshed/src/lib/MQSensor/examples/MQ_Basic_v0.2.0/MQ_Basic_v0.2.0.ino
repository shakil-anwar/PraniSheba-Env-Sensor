#include "MQ.h"
#include "EEPROM.h"

#define DO_CALIBRATION

#define MQ4_EEP_ADDR 0
#define MQ4_AIR_RS_R0_RATIO     4.4f
#define MQ4_PIN   A0

MQ mq4(MQ4_PIN);
void saveCalib(calib_t *cPtr);
void readCalib(calib_t *cPtr);
void setup()
{
  Serial.begin(9600);
  Serial.println("Heating Sensor..");
#if defined(DO_CALIBRATION)
  mq4.setXY(1000, 5000, 1, 0.6);
  mq4.setR(1000, MQ4_AIR_RS_R0_RATIO);
  mq4.runCalib(saveCalib);
#else
  mq4.beginFromMem(readCalib);
#endif
}

void loop()
{
  Serial.print(F(" MQ4 PPM: ")); Serial.println(mq4.getPPM());
  delay(1000);
}

void saveCalib(calib_t *cPtr)
{
  Serial.println(F("Calib Saved"));
  uint8_t *ptr = (uint8_t*)cPtr;
  for(uint8_t i = 0; i<sizeof(calib_t);i++)
  {
    EEPROM.update(MQ4_EEP_ADDR+i, *(ptr+i));
  }
  mq4.printCalib(cPtr);
}
void readCalib(calib_t *cPtr)
{
  Serial.println(F("Calib Rading.."));
  uint8_t *ptr = (uint8_t*)cPtr;
  for(uint8_t i = 0 ; i< sizeof(calib_t); i++)
  {
    *(ptr+i) = EEPROM.read(MQ4_EEP_ADDR+i);
  }
  mq4.printCalib(cPtr);
}
