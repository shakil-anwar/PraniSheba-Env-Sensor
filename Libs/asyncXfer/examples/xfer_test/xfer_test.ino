#include "asyncXfer.h"

void sendNrf(const void *data);
void *readMemQ();
int ackWait();

void setup() 
{
  Serial.begin(9600);
  xferBegin(readMemQ,sendNrf,ackWait);
  xferReady();
}

void loop() 
{
  
}
