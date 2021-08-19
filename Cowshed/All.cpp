#include "All.h"
#include "EEPROM.h"

enum bsSendState_t
{
  BS_SEND_WAIT,
  BS_IS_CONNECTED,
  BS_SEND,
  BS_SEND_END,
};

mainState_t mainState;

enum bsSendState_t _bsSendState;
enum tdmSyncState_t tdmSyncState;
void printRunState();
bool isBsConnected();
bool isMySlot();

runState_t runState;

volatile uint32_t _prevRunSec;
volatile uint32_t _nowSec;
uint32_t _nextSlotUnix;
uint32_t _nextSlotEnd;
bool _nrfSendOk;
#if !defined(DEVICE_HAS_LOG)
int16_t rfFailCount;
#endif

Task taskNrfStatus(5, &nrfWhichMode);
void system_setup(void)
{
#if !defined(DEVICE_HAS_LOG)
  rfFailCount = 0;
#endif
  tdmSyncState = TDM_UNSYNCED;
  Serial.begin(SERIAL_SPEED);
  SerialBegin(SERIAL_SPEED);  //supporting serial c library
  gpioBegin(); //This function has to call first to set sensitive pin like cs pin of spi
  Serial.println("[pS Env Sensor v0.6.5]");



  radio_begin();
#if defined(DEVICE_HAS_RTC)
  rtcBegin();
  rtAttachRTC(rtcGetSec, rtcUpdateSec);
#endif
  TimersBegin();

  deviceBegin();
  objectsBegin();
#if defined(DEVICE_HAS_LOG)
  initiateLog();
#endif

  //confsetting has to call after deviceBegin, because it operate on flash and sensor
  confSetting(CONFIG_BTN_PIN, configRead, configSave);

  scheduler.addTask(&taskNrfStatus);
  // wdtEnable(8000);
  BUZZER_ON();
  delay(1000);
  BUZZER_OFF();
  Serial.println("Setup Done.");
}

void startDevice()
{
  runState = RUN_WAIT;
  _bsSendState = BS_SEND_WAIT;
  radioStart();
  // wdtStart();
#if defined(FACTORY_RESET)
   nrfTxConfigReset(&nrfConfig, NRF_CONFIG_ROM_ADDR, eepromUpdate);
#endif
  
#if defined(DATA_ACQUIRE_INTERVAL)
  updateDataInterval(DATA_ACQUIRE_INTERVAL);
#else
  updateDataInterval(config.sampInterval);
#endif
}

void deviceRunSM()
{
  switch (runState)
  {
    case RUN_WAIT:
      _nowSec = second();
      if (_nowSec -  _prevRunSec >= DATA_TRASNFER_INTERVAL)
      {
        _prevRunSec = _nowSec;
        runState = RUN_CHK_BS_CONN;
        Serial.println(F("Time to data send"));
      }
      break;
    case RUN_CHK_BS_CONN:
      if (isBsConnected())
      {
        nrfTxReady(&nrfConfig);
        xferReady();
        runState = RUN_TX_XFER;
      }
      else
      {
        runState = RUN_WAIT;
        Serial.println(F("<==BS Not Connected==>"));
      }
      break;
    case RUN_TX_XFER:
      Serial.println(F("run : RUN_TX_XFER"));
      _nrfSendOk = xferSendLoopV2();
      if (_nrfSendOk)
      {
        runState = RUN_WAIT;
        Serial.println(F("Done and RUN_END_TRANSFER"));
      }
      else
      {
        runState = RUN_WAIT;
        Serial.println(F("Failed and wait"));
        //runState = RUN_CHK_BS_CONN;
      }
      break;
    default :
      Serial.println(F("-->Accidental Default mode"));
      runState = RUN_WAIT;
      break;
  }
  printRunState();

}


void bsSendSm()
{
  switch (_bsSendState)
  {
    case BS_SEND_WAIT:
      if (second() >= _nextSlotSec)
      {
          _nextSlotEnd = _nextSlotSec+(uint32_t)nrfConfig.momentDuration;
          _bsSendState = BS_IS_CONNECTED;
      }
      break;
    case BS_IS_CONNECTED:
      // Serial.println(F("run : BS_IS_CONNECTED"));
      if (isMySlot())
      {
#if defined(DEVICE_HAS_LOG)
        sensorLog.slotMissed = 1;
#else
        rfFailCount = 0;
#endif
        
        // nrfTxReady(&nrfConfig);

        nrfTxSetModeClient(BS_DATA,&nrfConfig);
        xferReady();
        _bsSendState = BS_SEND;
      }
      else
      {
#if defined(DEVICE_HAS_LOG)        
        sensorLog.slotMissed++;
        if(sensorLog.slotMissed == 0)
        {
          sensorLog.slotMissed = 1;
        }
#else
        rfFailCount++;
#endif
        if(tdmSyncState == TDM_CONFIG_CHANGED)
        {
          mainState = SYNC_RF;
        }
        _bsSendState = BS_SEND_END;
        Serial.println(F("<==BS Not Connected==>"));
      }
      break;
    case BS_SEND:
      // Serial.println(F("run : BS_SEND"));
      if(second() <= _nextSlotEnd)
      {
        _nrfSendOk = xferSendLoop();
      }
      else
      {
        Serial.println(F("BS_SEND>Slot Timeout"));
        _nrfSendOk = false;
      }
      
      
      if (_nrfSendOk)
      {
        Serial.print(F(".."));
      }
      else
      {
        Serial.println(F("All Not Sent"));
        _bsSendState = BS_SEND_END;
      }
      break;
    case BS_SEND_END:
#if defined(DEVICE_HAS_LOG)
        saveLog();
#endif
      _nextSlotSec = calcNextSlotUnix(second(), &nrfConfig);
      _bsSendState = BS_SEND_WAIT;
      break;
  }
}
//
//void bsSendSm()
//{
//  if (second() >= _nextSlotSec)
//  {
//    if (isMySlot())
//    {
//      nrfTxReady(&nrfConfig);
//      xferReady();
//      _nrfSendOk = xferSendLoopV3();
//      if (_nrfSendOk)
//      {
//        Serial.println(F("All Send Ok"));
//      }
//      else
//      {
//        Serial.println(F("All Not Sent"));
//      }
//      _nextSlotSec = calcNextSlotUnix(second(), &nrfConfig);
//    }
//  }
//}

bool isBsConnected()
{
  int8_t tryCount = 1;
  do
  {
    if (nrfPing())
    {
      return true;
    }
  } while (--tryCount);
  return false;
}


bool isMySlot()
{
  
  int8_t tryCount = 5;
  uint32_t uTime;
  pong_t pong;
  do
  {
    uTime = nrfPingSlot(config.deviceId, nrfConfig.slotId, &pong);
    int32_t delayTime = (int32_t)second();
    delayTime = (int32_t)((uint32_t)delayTime-pong.second);
    
    
    if(uTime)
    {
      if(abs(delayTime)>1)
      {
        rtSync(pong.second);
      }
      if (pong.isConfigChanged != 1)
      {
        if(abs(delayTime) < nrfConfig.perNodeInterval)
        {
          if(delayTime > 0)
          {
            delayTime = delayTime*1000;
            delay((int16_t)delayTime);
            // return true;
          }
          // else{
          //   Serial.println(F("SyncSlot>>Time gap"));
          // }
          tdmSyncState = TDM_SYNCED;
          
          return true;
        }
        else
        {
          tdmSyncState = TDM_SLOT_MISSED;
        }
      }
      else
      {
        tdmSyncState = TDM_CONFIG_CHANGED;
        return false;
      }
      return false;
    }
    
    
//     if (uTime > 1)
//     {
//       //update time 
// //      uTime = uTime-2;
//       tdmSyncState = TDM_SYNCED;
//       if(abs((int32_t)(second()-uTime))>1)
//       {
//         Serial.println(F(">>>>>>>>>>>>>>>.Time gap"));
//         rtSync(uTime);
//       }
//       return true;
//     }
//     else if(uTime == 1)
//     {
//       if(pong.isConfigChanged == 1)
//       {
//         tdmSyncState = TDM_CONFIG_CHANGED;
//       }
//       else
//       {
//         tdmSyncState = TDM_SYNCED;
//         rtSync(pong.second);
//       }
//       return false;
//     }
//     else{
//       tdmSyncState = TDM_SLOT_MISSED;
//     }
    // delay(100);
  } while (--tryCount);

  return false;
}

void printRunState()
{
  switch (runState)
  {
    case RUN_WAIT:
      //      Serial.println(F("runState : RUN_WAIT"));
      break;
    case RUN_CHK_BS_CONN:
      Serial.println(F("runState : CHK_BS_CONN"));
      break;
    case RUN_TX_XFER:
      Serial.println(F("runState : TX_XFER"));
      break;
  }
}

bool isHardwareOk()
{
  Serial.println(F("<--Hardware Status-->"));
  bool nrfOk = nrfIsRunning();
  Serial.print(F("NRF :")); Serial.println(nrfOk);
  bool rtcOk = rtcIsRunning();
  Serial.print(F("RTC: ")); Serial.println(rtcOk);
  Serial.print(F("Flash :")); Serial.println(true);
  Serial.print(F("Logic Power: ")); Serial.println(true);
  Serial.println(F("<------------->"));
  return (nrfOk);
}





void configSave(config_t *bootPtr)
{
  uint8_t *ptr = (uint8_t*)bootPtr;
  for (uint8_t i = 0 ; i < sizeof(config_t); i++)
  {
    EEPROM.update(MAIN_CONFIG_EEPROM_ADDR + i, *(ptr + i));
  }

  // eepromUpdate(MAIN_CONFIG_EEPROM_ADDR, ptr, sizeof(config_t));
}

void configRead(config_t *bootPtr)
{
  uint8_t *ptr = (uint8_t*)bootPtr;
  for (uint8_t i = 0 ; i < sizeof(config_t); i++)
  {
    *(ptr + i) = EEPROM.read(MAIN_CONFIG_EEPROM_ADDR + i);
  }

  // eepromRead(MAIN_CONFIG_EEPROM_ADDR, ptr, sizeof(config_t));
  //  return bootPtr;
}

//  if (millis() - prevModeMillis > 2000)
//  {
//    nrfWhichMode();
//    prevModeMillis = millis();
//  }
