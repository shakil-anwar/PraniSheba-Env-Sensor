#include "avr.h"
#include "All.h"
#include "radio.h"
#include "Timers.h"
#define SYNC_PING_DELAY_MS 5000

void printMainState(mainState_t mstate);

int i = 0;
bool stopWdt = false;


bool startRun = false;
void avrSetup()
{
  system_setup();
  // delay(5000);
  delay(500);
  mainState = CHECK_HARDWARE;
}

void avrloop()
{
  // if (Serial.available() > 0) 
  // {
  //   i = Serial.parseInt();
  //   Serial.print("I received: ");
  //   Serial.println(i);
  //   if (i == 1)
  //   {
  //     i = 0;
  //     stopWdt = true;
  //   }
  // }
  printSecond();
#if defined(LED_TEST)
  // ledTestAll();
#else
  // printMainState(mainState);
  switch (mainState)
  {
    case CHECK_HARDWARE:
      if (isHardwareOk())
      {
        if (confIsOk())
        {
          mainState = START_DEVICE;
          delay(config.deviceId);
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
      // mainState =RUN_LOOP;
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
#if defined(STAND_ALONE)
        tdmSyncState = TDM_UNSYNCED;
#else
        tdmSyncState = TDM_SYNCED;
#endif
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
#else //DEVICE_HAS_LOG
      if(rfFailCount > MAX_RF_DATA_SEND_RETRY)
      {
        rfFailCount = 0;
#endif //DEVICE_HAS_LOG
        tdmSyncState = TDM_CONFIG_CHANGED;
        mainState = SYNC_RF;
      }
#else //DEVICE_HAS_TDM
      deviceRunSM();
#endif //DEVICE_HAS_TDM

#if defined(DEVICE_HAS_FLASH_MEMORY)
      memqSaveMemPtr(&memq);
#endif  //DEVICE_HAS_FLASH_MEMORY
      rtLoop();
      // scheduler.run();
      scheduleTask();
      break;
    case STOP:
      //      mainState = CHECK_HARDWARE;
      break;
    default:
      mainState = CHECK_HARDWARE;
      break;
  }
#if defined(WDT_PIN)
  espWdtReset(); 
#endif
  // wdtReset();
#endif
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
#if defined(STAND_ALONE)
    Serial.println("Getting NTP Time");
    _nowSec = getNtpTime();
    // _nowSec = 1646733314;
#else
  _nowSec = nrfPing();
  
#endif
  Serial.print(F("NTP Time: ")); Serial.println(_nowSec);
  RT_SYNC_STATUS_t rtStatus = rtSync(_nowSec);
  if (rtStatus != UNSYNCED)
  {
    return true;
  }
  else
  {
#if !defined(STAND_ALONE)
    Serial.println(F("Mode====="));
    nrfWhichMode();
#endif
    delay(SYNC_PING_DELAY_MS);// ping after 2s interval
  }
  return false;
}

bool rfConfig()
{
#if defined(STAND_ALONE)
    tdmSyncState = TDM_SYNCED;
    setNRFconfig(); // set NRFconfig 
    return true;
#else
  bool conOk = nrfTxConfigHandler(config.deviceId, &nrfConfig, NRF_CONFIG_ROM_ADDR, 
                EEPROMRead, EEPROMUpdate, tdmSyncState == TDM_CONFIG_CHANGED);
  if (conOk)
  {
    _nextSlotSec = calcNextSlotUnix(second(), &nrfConfig);
    tdmSyncState = TDM_SYNCED;
    //    setNextSlotSec(slotSec);
    return conOk;
  }
  else
  {
    delay(SYNC_PING_DELAY_MS);// ping after 2s interval
  }
  return false;
#endif
}

void setNRFconfig()
{
    nrfConfig.momentDuration = 100;
    nrfConfig.perNodeInterval = nrfConfig.momentDuration - 10; // send data duration
    nrfConfig.slotId = 0;
    // nrfConfig.node[5];
    // nrfConfig.type;
    // nrfConfig.opcode;
    // nrfConfig.dataPipeLsbByte;
    // nrfConfig.checksum;
}