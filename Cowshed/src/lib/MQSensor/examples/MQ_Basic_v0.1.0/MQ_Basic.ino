#include "MQ.h"

#define MQ4_AIR_RS_R0_RATIO 1.0f
#define MQ135_AIR_RS_R0_RATIO 3.7f

MQ mq4(A0);
MQ mq135(A1);
/********Parameter for MQ-4***********/
//#define RL 1000.0
//#define R0 36888.0 //Calculated from Rs/R0 =1 for fresh air
//#define c (float)(0.7079)
//#define m (float)(-0.5693)


void setup()
{
  Serial.begin(9600);
  Serial.println("Heating Sensor..");
  //delay(20000);
  mq4.setGraphPoints(1000, 5000, 0.1, 0.04);
  mq4.setRl(1000);
  float R0 = mq4.calculateR0(MQ4_AIR_RS_R0_RATIO);//Call this function in the fresh air. This will set R0
  Serial.print(F("MQ4 | R0 : ")); Serial.println(R0);

  mq135.setGraphPoints(200, 40, 0.8, 1.5);
  mq135.setRl(1000);
  float mq135_R0 = mq135.calculateR0(MQ135_AIR_RS_R0_RATIO);
  Serial.print(F("MQ135 | R0 : ")); Serial.println(R0);

  Serial.println("Setup Done");
}

void loop()
{
  Serial.print(F("MQ4 | Rs: ")); Serial.print(mq4.Rs);
  Serial.print(F(" | PPM: ")); Serial.println(mq4.getPPM());

//  Serial.print(F("MQ135 | Rs: ")); Serial.print(mq135.Rs);
//  Serial.print(F(" | PPM: ")); Serial.println(mq135.getPPM());
//  delay(1000);        // delay in between reads for stability
 delay(1000);
}
