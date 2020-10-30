#include "All_Global.h"
#include "time.h"
void setup()
{
  system_setup();
}
void loop()
{
  scheduler.run();
}
