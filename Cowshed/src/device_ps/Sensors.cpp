#include "Sensors.h"
#include "pin.h"
#include "device.h"
#include "EEPROM.h"
// #include "MQ.h"
// #include <SHT21.h>
/*****************Gas Sensor Parameter******************/
#define MQ4_AIR_RS_R0_RATIO 		4.4f
#define MQ135_AIR_RS_R0_RATIO 		3.7f


void saveMq4Calib(calib_t *cPtr);
void readMq4Calib(calib_t *cPtr);

void saveMq135Calib(calib_t *cPtr);
void readMq135Calib(calib_t *cPtr);

float sensorValidate(float value);
void mqBegin();
void mqCalibrate();
void humSensorBegin();


SHT21 sht;

MQ mq4(MQ4_PIN);
MQ mq135(MQ135_PIN);




bool sensorBegin()
{
  Wire.begin();    // begin Wire(I2C)
  humSensorBegin();
  mqBegin();
}


void mqBegin()
{
  
  //  mq4.setGraphPoints(1000, 5000, 1, 0.57);
  // mq4.setGraphPoints(1, 1.82, 1000, 200);
  // mq4.setRl(1000);

  // mq135.setGraphPoints(2.65, 1, 10, 100);
  // mq135.setRl(1000);

#if defined(DO_CALIBRATION)
	// mq4.setXY(1000, 5000, 1, 0.6);
	// mq4.setR(1000, MQ4_AIR_RS_R0_RATIO);
	// mq4.runCalib(saveMq4Calib);

	// mq135.setXY(2.65, 1, 10, 100);
	// mq135.setR(1000, MQ135_AIR_RS_R0_RATIO);
	// mq135.runCalib(saveMq135Calib);
  sensorCalibrate();
#else
	mq4.beginFromMem(readMq4Calib);
	mq135.beginFromMem(readMq135Calib);
#endif

}

bool sensorCalibrate()
{
  mq4.setXY(1000, 5000, 1, 0.6);
	mq4.setR(1000, MQ4_AIR_RS_R0_RATIO);
	mq4.runCalib(saveMq4Calib);

	mq135.setXY(2.65, 1, 10, 100);
	mq135.setR(1000, MQ135_AIR_RS_R0_RATIO);
	mq135.runCalib(saveMq135Calib);
	return true;
}

float sensorValidate(float value)
{
  if(isfinite(value))
  {
  	return value;
  }
  else
  {
  	return 0;
  }
}

void humSensorBegin()
{
  Wire.begin();    // begin Wire(I2C)
}

float getHum()
{
#if defined(DEV)
  return config.deviceId;
#else
  return sensorValidate(sht.getHumidity());
#endif
}

float getTemp()
{
#if defined(DEV)
  return config.deviceId;
#else
  return sensorValidate(sht.getTemperature());
#endif
}

float getAmmonia()
{
#if defined(DEV)
  return config.deviceId;
  //  return (float)(random(10,50)*1.00);
#else
  return sensorValidate(mq4.getPPM());
#endif
}

float getMethane()
{
#if defined(DEV)
  return config.deviceId;
  //  return (float)(random(10,50)*1.00);
#else
  return sensorValidate(mq135.getPPM());
#endif
}


void saveMq4Calib(calib_t *cPtr)
{
  Serial.println(F("Calib Saving for mq4"));
  uint8_t *ptr = (uint8_t*)cPtr;
  for(uint8_t i = 0; i<sizeof(calib_t);i++)
  {
    EEPROM.update(MQ4_EEP_ADDR+i, *(ptr+i));
  }
  mq4.printCalib(cPtr);
}

void readMq4Calib(calib_t *cPtr)
{
  Serial.println(F("Calib Rading for mq4"));
  uint8_t *ptr = (uint8_t*)cPtr;
  for(uint8_t i = 0 ; i< sizeof(calib_t); i++)
  {
    *(ptr+i) = EEPROM.read(MQ4_EEP_ADDR+i);
  }
  mq4.printCalib(cPtr);
}

void saveMq135Calib(calib_t *cPtr)
{
  Serial.println(F("Calib Saving for mq135"));
  uint8_t *ptr = (uint8_t*)cPtr;
  for(uint8_t i = 0; i<sizeof(calib_t);i++)
  {
    EEPROM.update(MQ135_EEPROM_ADDR+i, *(ptr+i));
  }
  mq4.printCalib(cPtr);
}
void readMq135Calib(calib_t *cPtr)
{
  Serial.println(F("Calib Rading for mq135"));
  uint8_t *ptr = (uint8_t*)cPtr;
  for(uint8_t i = 0 ; i< sizeof(calib_t); i++)
  {
    *(ptr+i) = EEPROM.read(MQ135_EEPROM_ADDR+i);
  }
  mq4.printCalib(cPtr);
}
