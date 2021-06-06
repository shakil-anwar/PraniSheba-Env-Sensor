#include "tScheduler.h"
#include <TimerOne.h>

uint32_t milliSec = 0;
#define LED 13


void func1()
{
  Serial.println(F("Task One Done"));
}

void func2()
{
  Serial.println(F("Task Two Done"));
}

void func3()
{
  Serial.println(F("Task Three Done"));
}
void led_blink()
{
  Serial.println(F("LED BLINK"));
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
}

Task task1(2000, &func1);
Task taks2(2000, &func2);
Task task3(3000, &func3);
Task task4(1000, &led_blink);

Scheduler scheduler;
void setup()
{
  Serial.begin(115200);

  pinMode(LED,OUTPUT);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timeCounter);
  scheduler.addTask(&task1);
  scheduler.addTask(&taks2);
  scheduler.addTask(&task3);
  scheduler.addTask(&task4);
//  scheduler.begin(ms);
     scheduler.begin();
  Serial.println(F("Setup Done"));
}

void loop()
{
//  Serial.println(F("<---------Loop Start------------------->"));
  scheduler.run();
}
void timeCounter()
{
  milliSec++;
}

uint32_t ms()
{
  return milliSec;
}

