#include "Sensors.h"
#include "pin.h"
#include "device.h"
#include "EEPROM.h"
#if defined(ARDUINO_ARCH_ESP32)
  using namespace std;
#endif
// #include "MQ.h"
// #include <SHT21.h>
/*****************Gas Sensor Parameter******************/
#define MQ4_AIR_RS_R0_RATIO 		  4.4f
#define MQ135_AIR_RS_R0_RATIO 		3.7f


void saveMq4Calib(calib_t *cPtr);
void readMq4Calib(calib_t *cPtr);

void saveMq135Calib(calib_t *cPtr);
void readMq135Calib(calib_t *cPtr);

float sensorValidate(float value);
void mqBegin();
void mqCalibrate();
// void humSensorBegin();


SHT21 sht;

MQ mq4(MQ4_PIN);
MQ mq135(MQ135_PIN);




void sensorBegin()
{
  // Wire.begin();    // begin Wire(I2C)
  #if defined(ESP32_V010)  && defined(SOFTWARE_I2C)
  sht.begin(SHT21_SDA, SHT21_SCL); // Wire.begin() is called inside the sht.begin lib
  #else
  sht.begin();
  #endif
  sht.reset();
  // humSensorBegin();
  mqBegin();    // initializing variables for mq gas sensors
}


void mqBegin()
{
#if defined(ESP32_V010)
  pinMode(MQ4_PIN, INPUT);
  adcAttachPin(MQ4_PIN);
  pinMode(MQ135_PIN, INPUT);
  adcAttachPin(MQ135_PIN);
#endif
  mq4.setR(1000, MQ4_AIR_RS_R0_RATIO);
  mq135.setR(1000, MQ135_AIR_RS_R0_RATIO);
  mq4.attachSaveFuc(saveMq4Calib);
  mq135.attachSaveFuc(saveMq135Calib);
#if defined(DO_CALIBRATION)
  sensorCalibrate();
#else
	mq4.beginFromMem(readMq4Calib);
	mq135.beginFromMem(readMq135Calib);
#endif
}

bool sensorCalibrate()
{
  // Serial.println("sensor is being calibrated....");
  mq4.setXY(1000, 3000, 1, 0.68);
	mq4.setR(1000, MQ4_AIR_RS_R0_RATIO);
	mq4.runCalib(saveMq4Calib);

	mq135.setXY( 10, 100, 2.65, 1);
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

// void humSensorBegin()
// {
//   Wire.begin();    // begin Wire(I2C)

// }

float getHum()
{
// #if defined(DEV)
//   return config.deviceId;
// #else
//   return sensorValidate(sht.getHumidity());
// #endif

#if !defined(PROD_BUILD)
  return config.deviceId;
#else
#if defined(ESP32_V010) && defined(SOFTWARE_I2C)
  sht.begin(SHT21_SDA, SHT21_SCL); //required for soft i2c
#endif
  // digitalWrite(5,LOW);
  return sensorValidate(sht.getHumidity());
#endif
}

float getTemp()
{
// #if defined(DEV)
//   return config.deviceId;
// #else
//   return sensorValidate(sht.getTemperature());
// #endif

#if !defined(PROD_BUILD)
  return config.deviceId;
#else
#if defined(ESP32_V010) && defined(SOFTWARE_I2C)
  sht.begin(SHT21_SDA, SHT21_SCL); //required for soft i2c
#endif
  // digitalWrite(5,LOW); 
  return sensorValidate(sht.getTemperature());
#endif
}

float getAmmonia()
{
// #if defined(DEV)
//   return config.deviceId;
//   //  return (float)(random(10,50)*1.00);
// #else
//   return sensorValidate(mq4.getPPM());
// #endif

#if !defined(PROD_BUILD)
   return config.deviceId;
#else
  // return sensorValidate(mq135.getPPM());
  float sum = 0.0, val;
  int i;
  for(i = 0; i < 20; i++)
  {
    val = mq135.getPPM();
    sum += val;

  }
  return sensorValidate((sum / 20));
#endif
}

float getMethane()
{
// #if defined(DEV)
//   return config.deviceId;
//   //  return (float)(random(10,50)*1.00);
// #else
//   return sensorValidate(mq135.getPPM());
// #endif

#if !defined(PROD_BUILD)
   return config.deviceId;
#else
  // return sensorValidate(mq4.getPPM());
  float sum = 0.0, val;
  int i;
  for(i = 0; i < 20; i++)
  {
    val = mq4.getPPM();
    sum += val;
  }

  return sensorValidate((sum / 20));
#endif
}


void saveMq4Calib(calib_t *cPtr)
{
  Serial.println(F("Saving for mq4"));
  EEPROMUpdate(MQ4_EEP_ADDR, (uint8_t*)cPtr,sizeof(calib_t));
  mq4.printCalib(cPtr);
}

void readMq4Calib(calib_t *cPtr)
{
  Serial.println(F("Rading for mq4"));
  EEPROMRead(MQ4_EEP_ADDR,(uint8_t*)cPtr,sizeof(calib_t));
  mq4.printCalib(cPtr);
}

void saveMq135Calib(calib_t *cPtr)
{
  Serial.println(F("Saving for mq135"));
  EEPROMUpdate(MQ135_EEPROM_ADDR, (uint8_t*)cPtr,sizeof(calib_t));
  mq135.printCalib(cPtr);
}

void readMq135Calib(calib_t *cPtr)
{
  Serial.println(F("Rading for mq135"));
  EEPROMRead(MQ135_EEPROM_ADDR,(uint8_t*)cPtr,sizeof(calib_t));
  mq135.printCalib(cPtr);
}
