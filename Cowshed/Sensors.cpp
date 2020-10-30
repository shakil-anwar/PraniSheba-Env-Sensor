#include "dataSchema.h"
#include "MQ.h"
#include "param.h"
#include "pin.h"
#include <SHT21.h> 
SHT21 sht; 

MQ mq4(MQ4_PIN);
MQ mq135(MQ135_PIN);

void mqBegin()
{
  Wire.begin();    // begin Wire(I2C)
  mq4.setGraphPoints(1000, 5000, 0.1, 0.04);
  mq4.setRl(1000);
  
  mq135.setGraphPoints(200, 40, 0.8, 1.5);
  mq135.setRl(1000);
  
}
void mqCalibrate()
{
  Serial.println(F("Calibrating MQ Sensors.."));
  float R0 = mq4.calculateR0(MQ4_AIR_RS_R0_RATIO);//Call this function in the fresh air. This will set R0
  Serial.print(F("MQ4 | R0 : ")); Serial.println(R0);

  float mq135_R0 = mq135.calculateR0(MQ135_AIR_RS_R0_RATIO);
  Serial.print(F("MQ135 | R0 : ")); Serial.println(R0);
  delay(2000);
}

void humSensorBegin()
{
  
}
float getHum()
{
  return sht.getHumidity();
}

float getTemp()
{
  return sht.getTemperature(); 
}

float getAmmonia()
{
  return (float)(random(10,50)*1.00);
}

float getMethane()
{
  return (float)(random(10,50)*1.00);
}
