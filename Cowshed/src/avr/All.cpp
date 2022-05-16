#include "All.h"
#if defined(FRAM_V010)
#include "../common_lib/FM24I2C/FM24I2c.h"
#else
#include <EEPROM.h>
#endif
#include "..\wifi\iotClient.h"

volatile uint32_t task1Time = 0;
volatile uint32_t task2Time = 0;
volatile uint32_t task3Time = 0;
volatile uint32_t wdtResetTime = 0;
volatile uint32_t railVTime = 0;

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

// Task taskNrfStatus(5, &nrfWhichMode);
void system_setup(void)
{
#if !defined(DEVICE_HAS_LOG)
  rfFailCount = 0;
#endif
  tdmSyncState = TDM_UNSYNCED;
  Serial.begin(SERIAL_SPEED);
  SerialBegin(SERIAL_SPEED);  //supporting serial c library
  gpioBegin(); //This function has to call first to set sensitive pin like cs pin of spi
  Serial.println("[pS Env Sensor v0.8.0]");
  
  spi_begin(SPI_SPEED);
#if !defined(STAND_ALONE)
  radio_begin();    // spi - C began here
#endif

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
#if defined(SUBCRIPTION_ON)
  initiateDummySucbscriptionData();
  // loadSubcriptionPlan();
#endif
#if defined(WDT_PIN)
  espWdtReset(true); 
#endif
  //confsetting has to call after deviceBegin, because it operate on flash and sensor
  confSetting(CONFIG_BTN_PIN, configRead, configSave);
  // scheduler.addTask(&taskNrfStatus);
  // wdtEnable(8000);
  BUZZER_ON();
  // delay(1000);
  delay(500);
  BUZZER_OFF();
  Serial.println("Setup Done.");
}

void startDevice()
{
  runState = RUN_WAIT;
  _bsSendState = BS_SEND_WAIT;
#if !defined(STAND_ALONE)
  radioStart();
#endif
  // wdtStart();
#if defined(FACTORY_RESET)
   nrfTxConfigReset(&nrfConfig, NRF_CONFIG_ROM_ADDR, EEPROMUpdate);
#endif
  
// #if defined(DATA_ACQUIRE_INTERVAL)
//   updateDataInterval(DATA_ACQUIRE_INTERVAL);
// #else
//   updateDataInterval(config.sampInterval);
// #endif
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
      // _nrfSendOk = xferSendLoopV3();
      _nrfSendOk = xferSendLoop();
      if (_nrfSendOk)
      {
        // runState = RUN_WAIT;
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
        _nextSlotEnd = _nextSlotSec+(uint32_t)nrfConfig.perNodeInterval;
        _bsSendState = BS_IS_CONNECTED;
        #if defined(STAND_ALONE)
        // espIotState = IOT_BEGIN;
        // startUpStatus = false;
        // Serial.println("Set espIotSate IOT_BEGIN");
        #endif    
      }
      break;
    case BS_IS_CONNECTED: 
      // Serial.println(F("run : BS_IS_CONNECTED"));
      if (isMySlot()) // have to use this func for time sync device
      {
        #if defined(DEVICE_HAS_LOG)
        sensorLog.slotMissed = 1;
        #else
        rfFailCount = 0;
        #endif //DEVICE_HAS_LOG
        
        // nrfTxReady(&nrfConfig);

#if !defined(STAND_ALONE)
        nrfTxSetModeClient(BS_DATA,&nrfConfig);
#endif //STAND_ALONE
        xferReady();
        _bsSendState = BS_SEND;
      }
#if !defined(STAND_ALONE)
      else
      {
#if defined(DEVICE_HAS_LOG)        
        sensorLog.slotMissed++;
        if(sensorLog.slotMissed == 0)
        {
          sensorLog.slotMissed = 1;
        }
#else //DEVICE_HAS_LOG
        rfFailCount++;
#endif //DEVICE_HAS_LOG
        if(tdmSyncState == TDM_CONFIG_CHANGED)
        {
          mainState = SYNC_RF;
        }
        _bsSendState = BS_SEND_END;
        Serial.println(F("<==BS Not Connected==>"));
      }
#endif //STAND_ALONE
      break;
    case BS_SEND:
      // Serial.println(F("run : BS_SEND"));
#if defined(STAND_ALONE)
      _nrfSendOk = xferSendLoop();
#else
      if(second() <= _nextSlotEnd)
      {
        _nrfSendOk = xferSendLoop();
      }
      else
      {
        Serial.println(F("BS_SEND>Slot Timeout"));
        _nrfSendOk = false;
      }     
#endif    
      if (_nrfSendOk)
      {
        // Serial.print(F(".."));
      }
      else
      {
        if(memqAvailable(&memq)>0)
        {
          Serial.println(F("All Not Sent"));
        }
        else
        {
          Serial.println(F("All Sent"));
#if defined(STAND_ALONE)
          netState.dataSendReady = 0;
          // espIotState = IOT_IDLE;
#if defined(GSM_V010)
          // if(gsmRadioOff())
          // {
          //   Serial.println("GSM radio off");
          // }
#endif // GSM_V010
          Serial.println("Set espIotSate IDLE");
#endif //STAND_ALONE
        }
        
        _bsSendState = BS_SEND_END;
      }
      break;
    case BS_SEND_END:
#if defined(DEVICE_HAS_LOG)
        saveLog();
#endif
      _nextSlotSec = calcNextSlotUnix(second(), &nrfConfig);
      _bsSendState = BS_SEND_WAIT;
#if defined(STAND_ALONE)
      tdmSyncState = TDM_UNSYNCED;
#endif 
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
#if defined(STAND_ALONE)
  if (tdmSyncState != TDM_SYNCED)
  {
    do
    {  
      uTime =getNtpTime();
      int32_t delayTime = (int32_t)second();
      delayTime = (int32_t)((uint32_t)delayTime-uTime);
      if(uTime)
      {
        if(abs(delayTime)>1)
        {
          rtSync(uTime);
          // return true;
        }
        // return true;
        tdmSyncState = TDM_SYNCED;
        break;
      }
      else
      {
        tdmSyncState = TDM_UNSYNCED;
        // Serial.println("isMyslot missed.........???");
      }
    } while (--tryCount);
  }

  if(netState.mqttStatus == 1 /*&& netState.dataSendReady == 1*/)
  {
    Serial.println("device ready to send data............>>>");
    return true;
  }
  else
  {
    return false;
  }
  
  
#else
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
  } while (--tryCount);

  return false;
#endif //stand_alone
}


void scheduleTask()
{
  // Task 1
  #if !defined(SOFTWARE_I2C)
  // runTask(updateDisplay,10,&task1Time);
  #else
    // runTask(espWdtReset,5,&wdtResetTime);
    runTask(printRailVoltage,10, &railVTime);
  #endif
  // Task 2
  runTask(schemaReadSensors,config.sampInterval,&task2Time);
  // Task 3
  // #if defined(ESP32_V010)
  // runTask(nrfWhichMode2,5,&task3Time);
  // #else
  #if !defined(STAND_ALONE)
  runTask(nrfWhichMode,5,&task3Time);
  #endif //STAND_ALONE
  // #endif
}


void runTask( void(*func)(void), uint32_t interval,volatile uint32_t *prevtime)
{
  if((second() - *prevtime)>=interval)
  {
    *prevtime = second(); 
    if(func)
    {
      func();
    }
    // *prevtime = second();   
  }


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
  #if defined(STAND_ALONE)
  bool nrfOk = true;
  #else //STAND_ALONE
  bool nrfOk = nrfIsRunning();
  Serial.print(F("NRF :")); Serial.println(nrfOk);
  #endif //STAND_ALONE
  bool rtcOk = rtcIsRunning();
  Serial.print(F("RTC: ")); Serial.println(rtcOk);
  bool flashOk =  isFlashRunning();
  Serial.print(F("Flash :")); Serial.println(flashOk);
  Serial.print(F("Logic Power: ")); Serial.println(true);
  Serial.println(F("<------------->"));
  return (nrfOk);
}





void configSave(config_t *bootPtr)
{
#if defined(FRAM_V010)
  fram.write(MAIN_CONFIG_EEPROM_ADDR,(void *)bootPtr,sizeof(config_t));
#else
  EEPROMUpdate(MAIN_CONFIG_EEPROM_ADDR,(uint8_t*)bootPtr,sizeof(config_t));
#endif
}

void configRead(config_t *bootPtr)
{
#if defined(FRAM_V010)
  fram.read(MAIN_CONFIG_EEPROM_ADDR,(void *)bootPtr,sizeof(config_t));
#else
  EEPROMRead(MAIN_CONFIG_EEPROM_ADDR, (uint8_t*)bootPtr,sizeof(config_t));
#endif
}

//  if (millis() - prevModeMillis > 2000)
//  {
//    nrfWhichMode();
//    prevModeMillis = millis();
//  }
