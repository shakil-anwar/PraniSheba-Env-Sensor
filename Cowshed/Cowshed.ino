#include "All.h"
#include "radio.h"
void printMainState(mainState_t mstate);
mainState_t mainState;
bool startRun = false;
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
      _nowSec = nrfPing();
      if (_nowSec)
      {
        nrfTxAddrHandler(readAddr, saveAddr);//read addr from memory
        rtSync(_nowSec);// synctime | this will begin data acquire scheduler
        nrfTxReady();
        mainState = RUN_LOOP;
        _nowSec = second();
        _prevRunSec = _nowSec;
        Serial.println(F("Starting Operation"));
      }
      break;
    case RUN_LOOP:
      deviceRunSM();
      rtLoop();
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
    case RUN_LOOP:
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
