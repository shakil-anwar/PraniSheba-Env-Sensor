#include "All.h"
#include "EEPROM.h"

void printRunState();
bool isBsConnected();

runState_t runState;

volatile uint32_t _prevRunSec;
volatile uint32_t _nowSec;
bool _nrfSendOk;
uint32_t prevModeMillis = 0 ;

Task taskNrfStatus(5, &nrfWhichMode);
void system_setup(void)
{
  Serial.begin(250000);

  radio_begin();
  rtcBegin();
  rtAttachRTC(rtcGetSec, rtcUpdateSec);
  rtBegin();

  deviceBegin();
  objectsBegin();

  scheduler.addTask(&taskNrfStatus);
  wdtEnable(8000);
  delay(1000);
  Serial.println("Setup Done.");
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

void startDevice()
{
  radioStart();
  wdtStart();
#if defined(FACTORY_RESET)
  nrfTxAddrReset(saveAddr);
#endif
  runState = RUN_WAIT;
}

void deviceRunSM()
{
#if defined(DEVICE_HAS_FLASH_MEMORY)
  memQ.saveLoop();
#endif
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
        nrfTxReady();
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
        Serial.println(F("Going to Wait"));
      }
      else
      {
        runState = RUN_CHK_BS_CONN;
      }
      break;
    default :
      Serial.println(F("-->Accidental Default mode"));
      runState = RUN_WAIT;
      break;
  }
  printRunState();


  //  if (millis() - prevModeMillis > 2000)
  //  {
  //    nrfWhichMode();
  //    prevModeMillis = millis();
  //  }
}

bool isBsConnected()
{
  int8_t tryCount = 3;
  do
  {
    if(nrfPing())
    {
      return true;
    }
  }while(--tryCount);
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


void saveAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Saving.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for (uint8_t i = 0; i < sizeof(addr_t); i++)
  {
    EEPROM.update(ROM_ADDR_FOR_TXD + i, *(ptr + i));
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}

void readAddr(addr_t *addrPtr)
{
  Serial.println(F("NRF EEPROM Reading.."));
  uint8_t *ptr = (uint8_t*)addrPtr;
  for (uint8_t i = 0 ; i < sizeof(addr_t); i++)
  {
    *(ptr + i) = EEPROM.read(ROM_ADDR_FOR_TXD + i);
  }
  nrfDebugBuffer(ptr, sizeof(addr_t));
}
