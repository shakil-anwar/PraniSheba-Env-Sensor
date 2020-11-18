#include "All_Global.h"
#include "time.h"

#include "radio.h"
#include "dataSchema.h"

uint32_t waitPrevMillis = 0;

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
  wdtReset();
}

void sampleSendNrf()
{
  uint32_t currentMillis = millis();
  uint32_t prevMillis = currentMillis;
  nrf_irq_state_t irqState;
  do
  {
    Serial.println(F("Sending data.."));
    payload_t *pld = (payload_t*)getSensorsData();
    nrfWrite((uint8_t*)pld, sizeof(payload_t));
    nrfStartTransmit();
    waitPrevMillis = millis();
    do
    {

      irqState = waitAck();
      currentMillis = millis();
      if (currentMillis - waitPrevMillis > 15)
      {
        break;
      }
    } while (irqState != NRF_SUCCESS && irqState != NRF_FAIL);
    //currentMillis - prevMillis < 1000
  } while (0);
  delay(3000);

}
