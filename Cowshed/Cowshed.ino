#include "All_Global.h"
#include "time.h"

#include "radio.h"
#include "dataSchema.h"



void setup()
{
  system_setup();
}
void loop()
{
    getRtcTime();
    delay(2000);

  //  scheduler.run();
  //  payloadStateMachine();
//  sampleSendNrf();
//  delay(100);
  wdtReset();
}

void sampleSendNrf()
{
  Serial.print(F("Sending data.."));
  
  payload_t *pld = (payload_t*)getSensorsData();
  nrfWrite((uint8_t*)pld, sizeof(payload_t));
  nrfStartTransmit();
  
 
  uint8_t currentMillis = millis();
  uint32_t waitPrevMillis = currentMillis;
  nrf_irq_state_t irqState;
  do
  {
    irqState = waitAck();
    Serial.print(F("IRQ STATE: "));Serial.println(irqState);
    if (irqState == NRF_SUCCESS)
    {
      Serial.println(F("Success"));
      break;
    }
    else if (irqState == NRF_FAIL)
    {
      Serial.println(F("MAX_RT Failed"));
      break;
    }
   currentMillis = millis();
  } while ((currentMillis - waitPrevMillis)<10);
  Serial.println();
}
