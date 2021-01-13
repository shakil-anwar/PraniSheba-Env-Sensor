#include "All.h"
#include "radio.h"

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
//      nrfTxAddrRestore(2);
//      rtSync(163456366);
//      nrfStandby1();
//      nrfTXStart();
//      mainState = DEVICE_RUN;
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
