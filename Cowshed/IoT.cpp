#include "IoT.h"

Scheduler scheduler;
void objectsBegin()
{
  xferBegin(readMem, sendNrf, ackWait);
  xferReady();
}
