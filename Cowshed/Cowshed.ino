#include "All_Global.h"
#include "time.h"
void setup()
{
  system_setup();
//  test_flash();
}
void loop()
{

//  delay(2000);
//  Serial.println("sv");
  wdtReset();
  scheduler.run();
  payloadStateMachine();
  
  
}
