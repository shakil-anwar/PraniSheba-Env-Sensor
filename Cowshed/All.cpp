#include "All.h"
#include "EEPROM.h"

enum bsSendState_t
{
  BS_SEND_WAIT,
  BS_IS_CONNECTED,
  BS_SEND,
  BS_SEND_END,
};

enum bsSendState_t _bsSendState;
void printRunState();
bool isBsConnected();
bool isMySlot();

runState_t runState;

volatile uint32_t _prevRunSec;
volatile uint32_t _nowSec;
uint32_t _nextSlotUnix;
bool _nrfSendOk;

Task taskNrfStatus(5, &nrfWhichMode);
void system_setup(void)
{
  Serial.begin(SERIAL_SPEED);
  SerialBegin(SERIAL_SPEED);  //supporting serial c library
  gpioBegin(); //This function has to call first to set sensitive pin like cs pin of spi
  radio_begin();
#if defined(DEVICE_HAS_RTC)
  rtcBegin();
  rtAttachRTC(rtcGetSec, rtcUpdateSec);
#endif
  TimersBegin();

  deviceBegin();
  objectsBegin();

  //confsetting has to call after deviceBegin, because it operate on flash and sensor
  confSetting(CONFIG_BTN_PIN, configRead, configSave);

  scheduler.addTask(&taskNrfStatus);
  wdtEnable(8000);
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
  wdtStart();
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
      _nrfSendOk = xferSendLoopV3();
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
        if(second() >= _nextSlotSec)
        {
          _bsSendState = BS_IS_CONNECTED;
        }
      }
      break;
    case BS_IS_CONNECTED:
      Serial.println(F("run : BS_IS_CONNECTED"));
      if (isMySlot())
      {
        // nrfTxReady(&nrfConfig);
        nrfTxSetModeClient(BS_DATA,&nrfConfig);
        xferReady();
        _bsSendState = BS_SEND;
      }
      else
      {
        _bsSendState = BS_SEND_END;
        Serial.println(F("<==BS Not Connected==>"));

      }
      break;
    case BS_SEND:
      Serial.println(F("run : BS_SEND"));
      _nrfSendOk = xferSendLoopV3();
      _bsSendState = BS_SEND_END;
      if (_nrfSendOk)
      {
        Serial.println(F("All Send Ok"));
      }
      else
      {
        Serial.println(F("All Not Sent"));
      }
      break;
    case BS_SEND_END:
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
  int8_t tryCount = 3;
  uint32_t uTime;
  do
  {
    uTime = nrfPingSlot(config.deviceId, nrfConfig.slotId);
    if (uTime)
    {
      //update time 
//      uTime = uTime-2;
      if(abs((int32_t)(second()-uTime))>1)
      {
        Serial.println(F(">>>>>>>>>>>>>>>>.Time gap"));
        rtSync(uTime);
      }
      return true;
    }
    else
    {
      delay(100);
    }
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
      Serial.println(F("runState : RUN_CHK_BS_CONN"));
      break;
    case RUN_TX_XFER:
      Serial.println(F("runState : RUN_TX_XFER"));
      break;
  }
}

bool isHardwareOk()
{
  Serial.println(F("<--Hardware Status-->"));
  bool nrfOk = nrfIsRunning();
  Serial.print(F("NRF :")); Serial.println(nrfOk);
  Serial.print(F("RTC: ")); Serial.println(true);
  Serial.print(F("Flash :")); Serial.println(true);
  Serial.print(F("Logic Power: ")); Serial.println(true);
  Serial.println(F("<------------------->"));
  return true;
}





void configSave(config_t *bootPtr)
{
  uint8_t *ptr = (uint8_t*)bootPtr;
  for (uint8_t i = 0 ; i < sizeof(config_t); i++)
  {
    EEPROM.update(MAIN_CONFIG_EEPROM_ADDR + i, *(ptr + i));
  }
}

void configRead(config_t *bootPtr)
{
  uint8_t *ptr = (uint8_t*)bootPtr;
  for (uint8_t i = 0 ; i < sizeof(config_t); i++)
  {
    *(ptr + i) = EEPROM.read(MAIN_CONFIG_EEPROM_ADDR + i);
  }
  //  return bootPtr;
}

//  if (millis() - prevModeMillis > 2000)
//  {
//    nrfWhichMode();
//    prevModeMillis = millis();
//  }
