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
//  scheduler.run();
//  payloadStateMachine();

  led_set_level(1,3);
  led_set_level(2,4);
  led_set_level(3,2);
  led_set_level(4,3);
  
  
}
