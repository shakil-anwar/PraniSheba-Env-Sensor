#include "IoT.h"
//#include "./src/Schema.h"
//#include "radio.h"


void objectsBegin()
{
  xferBegin(readMem, sendNrf, ackWait);
  xferReady();
}
