#include "All.h"
#include "radio.h"
#define SYNC_PING_DELAY_MS 5000

void printMainState(mainState_t mstate);

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
      if (isHardwareOk())
      {
        if (confIsOk())
        {
          mainState = START_DEVICE;
        }
        else
        {
          Serial.println(F("Device is not configured"));
          mainState = STOP;
        }
      }
      else
      {
        Serial.println(F("Hardware Problem"));
        mainState = STOP;
      }
      break;
    case START_DEVICE:
      startDevice();
      mainState = SYNC_DEVICE;
      break;
    case SYNC_DEVICE:
      if (syncTime())
      {
        // nrfTxAddrHandler(readAddr, saveAddr);//read addr from memory
        mainState = SYNC_RF;
        _nowSec = second();
        _prevRunSec = _nowSec;
      }
      break;
    case SYNC_RF:
      if (rfConfig())
      {
        mainState = RUN_LOOP;
        tdmSyncState = TDM_SYNCED;
      }else{
        tdmSyncState = TDM_UNSYNCED;
      }
      break;
    case RUN_LOOP:
#if defined(DEVICE_HAS_TDM)
      bsSendSm();

#if defined(DEVICE_HAS_LOG)  
      if((sensorLog.slotMissed % MAX_RF_DATA_SEND_RETRY) == 0)
      {
        sensorLog.slotMissed++;
#else
      if(rfFailCount > MAX_RF_DATA_SEND_RETRY)
      {
        rfFailCount = 0;
#endif
        mainState = SYNC_RF;
      }
#else
      deviceRunSM();
#endif

#if defined(DEVICE_HAS_FLASH_MEMORY)
      memqSaveMemPtr(&memq);
#endif
      rtLoop();
      scheduler.run();
      break;
    case STOP:
      //      mainState = CHECK_HARDWARE;
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
      //      Serial.println(F("m_STATE: STOP"));
      break;
  }
}


bool syncTime()
{
  _nowSec = nrfPing();
  Serial.print(F("NTP Time: ")); Serial.println(_nowSec);
  RT_SYNC_STATUS_t rtStatus = rtSync(_nowSec);
  if (rtStatus != UNSYNCED)
  {
    return true;
  }
  else
  {
    Serial.println(F("Mode====="));
    nrfWhichMode();
    delay(SYNC_PING_DELAY_MS);// ping after 2s interval
  }
  return false;
}

bool rfConfig()
{
  bool conOk = nrfTxConfigHandler(config.deviceId, &nrfConfig, NRF_CONFIG_ROM_ADDR, eepromRead, eepromUpdate);
  if (conOk)
  {

    _nextSlotSec = calcNextSlotUnix(second(), &nrfConfig);
    //    setNextSlotSec(slotSec);
    return conOk;
  }
  else
  {
    delay(SYNC_PING_DELAY_MS);// ping after 2s interval
  }
  return false;

}
