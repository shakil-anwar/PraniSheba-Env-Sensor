#include "All.h"
#include "radio.h"
//#include "./src/Schema.h"

mainState_t mainState;
void setup()
{
  system_setup();
  mainState = CHECK_HARDWARE;
}

void loop()
{
  switch (mainState)
  {
    case CHECK_HARDWARE:
      Serial.println(F("m_STATE: CHECK_HARDWARE"));
      if (isHardwareOk())
      {
        mainState = START_DEVICE;
      }
      else
      {
        mainState = STOP;
      }
      break;
    case START_DEVICE:
      Serial.println(F("m_STATE: START_DEVICE"));
      startDevice();
      mainState = SYNCHRONIZE;
      break;
    case SYNCHRONIZE:
      Serial.println(F("m_STATE: SYNCHRONIZE"));
      if (setDeviceConf())
      {
        mainState = DEVICE_RUN;
        nrfStandby1();
      }
      break;
    case DEVICE_RUN:
      deviceRunSM();
      scheduler.run();
      break;
    case STOP:
      Serial.println(F("m_STATE: STOP"));
      mainState = CHECK_HARDWARE;
      break;
    default:
      mainState = CHECK_HARDWARE;
      break;
  }
  wdtReset();
}
//
//void sampleSendNrf()
//{
//  Serial.print(F("Sending data.."));
//
//  //  payload_t *pld = (payload_t*)getSensorsData();
//  sensor_t sensor;
//  payload_t *pld = (payload_t*)getSensorsData(&sensor);
//  nrfWrite((uint8_t*)pld, sizeof(payload_t));
//  nrfStartTransmit();
//
//
//  uint8_t currentMillis = millis();
//  uint32_t waitPrevMillis = currentMillis;
//  nrf_irq_state_t irqState;
//  do
//  {
//    irqState = waitAck();
//    Serial.print(F("IRQ STATE: ")); Serial.println(irqState);
//    if (irqState == NRF_SUCCESS)
//    {
//      Serial.println(F("Success"));
//      break;
//    }
//    else if (irqState == NRF_FAIL)
//    {
//      Serial.println(F("MAX_RT Failed"));
//      break;
//    }
//    currentMillis = millis();
//  } while ((currentMillis - waitPrevMillis) < 10);
//  Serial.println();
//}

//    getRtcTime();
//    sampleSendNrf();
//    delay(2000);

//  scheduler.run();
//  payloadStateMachine();
