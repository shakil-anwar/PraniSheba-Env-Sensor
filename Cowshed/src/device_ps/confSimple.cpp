#include "confSimple.h"
#include "SerialString.h"
#include "Sensors.h"

const char ptext1[] PROGMEM = "1. Device ID";
const char ptext2[] PROGMEM = "2. Sampling Interval";
const char ptext3[] PROGMEM = "3. URL";

const char* const paramText[] PROGMEM =
{
  ptext1,
  ptext2,
  ptext3
};

bool configValidate(config_t *bootPtr);
void configPrint(config_t *bootPtr);


bool btnLongPressed(uint8_t pin);
void handlleDeviceCmd();
void handleParamSetting();
void printIntroText();
void printMainCmd();
void printDeviceCmd();
void printParamCmd(const char** const text, uint8_t totalCmd);



config_t config;

bool btnLongPressed(uint8_t pin)
{
  pinMode(pin, INPUT);
  bool press1 = digitalRead(pin);
  Serial.print(F("BTN status : ")); Serial.println(press1);
  if (press1 == !BTN_DEFAULT)
  {
    delay(2000);
    if (digitalRead(pin) == !BTN_DEFAULT)
    {
      Serial.print(F("BTN status : ")); Serial.println(press1);
      return true;
    }
  }
  return false;
}


void confSetting(uint8_t pin,memFun_t read, memFun_t save)
{
  read(&config);
  configPrint(&config);
  if (btnLongPressed(pin))
  {
    Serial.println(F("ENTERED SETUP"));
    printIntroText();
    
    printMainCmd();
    uint8_t maincmd;
    do
    {
      maincmd = getSerialCmd();
      if (maincmd == 1)
      {
        handlleDeviceCmd();
      }
      else if (maincmd == 2)
      {
        handleParamSetting();
      }
      else
      {
        Serial.println(F("Unknown main cmd"));
      }
    } while (maincmd);
    Serial.println(F("---->Exit Config"));
    if(configValidate(&config))
    {
      config.isSetupDone = SETUP_DONE_CONST;
    }
    save(&config);
    configPrint(&config);
  }
}

void handlleDeviceCmd()
{
  uint8_t subcmd = 0;
  printDeviceCmd();
  do
  {
    subcmd = getSerialCmd();
    switch (subcmd)
    {
      case 1:
        Serial.println(F("Erasing Flash.."));
        memQ.erase();
        break;
      case 2:
        Serial.println(F("Factory Resetting Device.."));
        
        break;
      case 3:
        Serial.println(F("Log On"));
        config.isDebugOff = false;
        break;
      case 4:
        Serial.println(F("Log Off"));
        config.isDebugOff = true;;
        break;
      case 5:
        Serial.println(F("Calibrating Sensor"));
        sensorCalibrate();
      break;
      default:
        if (subcmd == 0)
        {
          Serial.println(F("Exited Device Cmd"));
        }
        else
        {
          Serial.println(F("Unknown cmd"));
        }
        break;
    }
  } while (subcmd);
}

void handleParamSetting()
{

  printParamCmd(paramText, sizeof(paramText));
  char buf[32];
  uint8_t subcmd = 1;
  do
  {
    //    printParamName(subcmd);
    switch (subcmd)
    {
      case 1:
        Serial.print(F("Device Id : "));
        break;
      case 2:
        Serial.print(F("Sampling Interval : "));
        break;
    }
    getSerialString(buf);
    Serial.println(buf);
    switch (subcmd)
    {
      case 1:
        config.deviceId = atol(buf);
        Serial.println(F("Device id :")); Serial.println(config.deviceId);
        break;
      case 2:
        config.sampInterval = atoi(buf);
        Serial.println(F("Device interval:")); Serial.println(config.sampInterval);
        break;
    }
    subcmd++;
    if (subcmd > 2)
    {
      subcmd = 0;
    }
  } while (subcmd);
  Serial.println(F("Exit Param Setting"));
}



void printIntroText()
{
  Serial.println(F("|------------------------------------------------|"));
  Serial.println(F("|                   Gas Sensor                   |"));
  Serial.println(F("|        A Product of Adorsho Pranisheba         |"));
  Serial.println(F("|------------------------------------------------|"));
}


void printMainCmd()
{
  Serial.println(F("1. Device Setting"));
  Serial.println(F("2. Parameter Setting"));
  Serial.println(F("0. Exit Config"));
  //  return getSerialCmd();
}

void printDeviceCmd()
{
  Serial.println(F("1. Reset Flash Mem"));
  Serial.println(F("2. Factory Reset"));
  Serial.println(F("3. Log On"));
  Serial.println(F("4. Log Off"));
  Serial.println(F("5. Calibrate Sensor"));
  Serial.println(F("0. Exit Setup Menu"));
  //  return getSerialCmd();
}

void printParamCmd(const char** const text, uint8_t totalCmd)
{
  uint8_t total = totalCmd / 2;
  for (uint8_t i = 0; i < total; i++)
  {
    Serial.println((__FlashStringHelper*)text[i]);
  }
  //  return getSerialCmd();

  //  Serial.println((__FlashStringHelper*)text[0]);
  //  Serial.println((__FlashStringHelper*)text[1]);
  //  Serial.println((__FlashStringHelper*)text[2]);
}



bool configValidate(config_t *configPtr)
{
  return true;
}


void configPrint(config_t *configPtr)
{
 Serial.println(F("----------------------"));
 Serial.println(F("| Device Config Param |\r\n----------------------"));
 Serial.print(F("Device Id: "));Serial.println(configPtr -> deviceId);
 Serial.print(F("Samp Interval: "));Serial.println(configPtr -> sampInterval);
 Serial.print(F("Debug Off: "));Serial.println(configPtr ->isDebugOff);
 Serial.print(F("Registration: "));Serial.println(configPtr ->isSetupDone); 
 Serial.println(F("----------------------"));
}


//void printParamName(uint8_t cmdNo)
//{
//  switch (cmdNo)
//  {
//    case 1:
//      Serial.print(F("Device Id : "));
//      break;
//    case 2:
//      Serial.print(F("Sampling Interval : "));
//      break;
//  }
//}
