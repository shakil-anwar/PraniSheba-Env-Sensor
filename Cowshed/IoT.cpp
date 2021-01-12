#include "IoT.h"

Scheduler scheduler;
void objectsBegin()
{
  scheduler.begin(&second);
  xferBegin(deviceMemRead, deviceRfSend, deviceRfAckWait);
  xferReady();
}
