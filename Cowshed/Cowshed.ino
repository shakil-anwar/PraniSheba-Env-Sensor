#include "All.h"
#include "radio.h"
void printMainState(mainState_t mstate);

mainState_t mainState;
void setup()
{
  system_setup();
  mainState = CHECK_HARDWARE;
}

void loop()
{
  printMainState(mainState);
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
//      Serial.println(F("m_STATE: START_DEVICE"));
      startDevice();
      mainState = SYNC_DEVICE;
      break;
    case SYNC_DEVICE:
      uint32_t uTime = nrfPing();
      nrfTxAddrHandler(readAddr, saveAddr);//read addr from memory
      rtSync(uTime);// synctime | this will begin data acquire scheduler
      nrfTxReady();
      mainState = DEVICE_RUN;
      break;
    case DEVICE_RUN:
      deviceRunSM();
      scheduler.run();
      break;
    case STOP:
      mainState = CHECK_HARDWARE;
      break;
    default:
      mainState = CHECK_HARDWARE;
      break;
  }
  wdtReset();
}

void printMainState(mainState_t mstate)
{
  switch (mstate)
  {
    case CHECK_HARDWARE:
      Serial.println(F("m_STATE: CHECK_HARDWARE"));
      break;
    case START_DEVICE:
      Serial.println(F("m_STATE: START_DEVICE"));
      break;
    case SYNC_DEVICE:
      Serial.println(F("m_STATE: SYNC_DEVICE"));
      break;
    case DEVICE_RUN:
//      Serial.println(F("m_STATE: DEVICE_RUN"));
      break;
    case STOP:
      Serial.println(F("m_STATE: STOP"));
      break;
  }
}

//    case SYNCHRONIZE:
//      Serial.println(F("m_STATE: SYNCHRONIZE"));
//      if (setDeviceConf())
//      {
//        mainState = DEVICE_RUN;
//        nrfStandby1();
//      }
////      nrfTxAddrRestore(2);
////      rtSync(163456366);
////      nrfStandby1();
////      nrfTXStart();
////      mainState = DEVICE_RUN;
//      break;
