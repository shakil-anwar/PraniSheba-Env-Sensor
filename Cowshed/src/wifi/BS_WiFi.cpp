#include"BS_WiFi.h"


void wifiSetup()
{
  all_setup();
  delay(10);

}

void wifiLoop()
{
  espIoTClientLoop();
#if defined(STAND_ALONE)
  // otaLoop();
  mqttLoop();
#else
  espDataSendLoopMid();
#endif
#if defined(SUBCRIPTION_ON)
  // getSubscriptionStatus();
#endif
}
