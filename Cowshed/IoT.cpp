#include "IoT.h"
//#include "./src/Schema.h"
//#include "radio.h"

Scheduler scheduler;
void objectsBegin()
{
  xferBegin(readMem, sendNrf, ackWait);
  xferReady();
}
