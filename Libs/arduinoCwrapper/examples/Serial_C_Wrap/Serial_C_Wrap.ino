#include "test.h"

void setup() 
{
  Serial.begin(9600);
  testBegin();
  testPrint();
  Serial.println(F("Setup done"));
}

void loop()
{
  
}
