#include "all_global.h"
#include "pin.h"
#include "param.h"
#include "tScheduler.h"
#include "time.h"


void sendPayload();

Task task1(5, &sendPayload);
Scheduler scheduler;


//This will store all the sensor variables and function 
//returns the address of it.
packet_t packet;

//Sensor objects
MQ mq4(MQ4_PIN);
MQ mq135(MQ135_PIN);

//DHT dht(DHTPIN, DHTTYPE);

void system_setup(void)
{
  //begin led setup
  led_begin();
  //begin mq setup
  mq4.setGraphPoints(1000, 5000, 0.1, 0.04);
  mq4.setRl(1000);
  float R0 = mq4.calculateR0(MQ4_AIR_RS_R0_RATIO);//Call this function in the fresh air. This will set R0
  Serial.print(F("MQ4 | R0 : ")); Serial.println(R0);

  mq135.setGraphPoints(200, 40, 0.8, 1.5);
  mq135.setRl(1000);
  float mq135_R0 = mq135.calculateR0(MQ135_AIR_RS_R0_RATIO);
  Serial.print(F("MQ135 | R0 : ")); Serial.println(R0);


  radio_begin();
  setSensorId();

  scheduler.addTask(&task1);
  scheduler.begin(second);
  Serial.print(F("Sensor Id: "));Serial.println(packet.sid);
  Serial.println("Setup Done");
}
bool setSensorId()
{
  //Read sensor Id from EEPROM
  strcpy(packet.sid, "s12df");
}
packet_t *getSenorReading()
{
  packet.ammonia = random(2,20);
  packet.methane = random(2,20);
  packet.hum = random(2,20);
  packet.temp = random(2,20);
  return &packet;
}

void sendPayload()
{
  Serial.println(F("Payload sending.."));
}
