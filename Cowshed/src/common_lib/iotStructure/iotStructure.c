#include "iotStructure.h"

#if defined(PROD_BUILD)
    #include "../arduinoCwrapper/Serial.h"
#else
    #include "Serial.h"
#endif


struct netState_t netState;
struct bsParam_t bsParam;
struct subscriptionParam_t subscribeParam;

void printBolus(struct bolusXYZ_t *bolus)
{
  for (uint8_t i = 0; i < BOLUS_SAMPLE_IN_PACKET ; i++)
  {
    SerialPrintF(P("X :"));      SerialPrintU8(bolus -> x[i]);
    SerialPrintF(P(" | Y :"));   SerialPrintU8(bolus -> y[i]);
    SerialPrintF(P(" | Z :"));   SerialPrintlnU8(bolus -> z[i]);
  }
}
void printGasSensor(struct gasSensor_t *gas)
{
    SerialPrintF(P("Temp :"));   SerialPrintFloat(gas -> temp,2);
    SerialPrintF(P(" | Hum :"));   SerialPrintlnFloat(gas -> hum,2);
}


void printBsLogData(struct bslog_t *bsLogPtr)
{
  SerialPrintlnF(P("---print Base Station Log----"));
  // SerialPrintF("type: "); SerialPrintlnF(bsLogPtr -> header.type);
  SerialPrintF(P("cks: ")); SerialPrintlnU8(bsLogPtr -> header.checksum);
  // SerialPrintF("h.id: "); SerialPrintlnF(bsLogPtr -> header.id);
  SerialPrintF(P("isNetOk: ")); SerialPrintlnU8(bsLogPtr -> isNetOk);
  SerialPrintF(P("railVoltage: ")); SerialPrintlnFloat(bsLogPtr -> railVoltage,2);
  SerialPrintF(P("errorCode: ")); SerialPrintlnU8(bsLogPtr ->  errorCode);
  SerialPrintF(P("hardwareError: ")); SerialPrintlnU8(bsLogPtr ->  hardwareErrorCode);
  SerialPrintF(P("radioStat: ")); SerialPrintlnU8(bsLogPtr ->  radioStatus);
  SerialPrintF(P("syncType: ")); SerialPrintlnU8(bsLogPtr ->  syncType);
  SerialPrintF(P("rsCount :")); SerialPrintlnU16(bsLogPtr ->  restartCount);
  SerialPrintF(P("totalSlotTime :")); SerialPrintlnU16(bsLogPtr ->  totalSlotTime);
  SerialPrintF(P("flashAvailable :")); SerialPrintlnU32(bsLogPtr ->  flashAvailablePackets);
}



void configPrint(bsConfig_t *configPtr)
{
  SerialPrintlnF(P("----------------------"));
  SerialPrintlnF(P("| Device Config Param |\r\n----------------------"));
  SerialPrintF(P("BS Id: "));
  SerialPrintlnU16(configPtr->bsId);
  SerialPrintF(P("BS Moment in Second: "));
  SerialPrintlnU16(configPtr->momentDuration);
  SerialPrintF(P("Pipe0 Addr: "));
  SerialPrintU8(configPtr->bsPipe0Addr[0]); SerialPrintF(P(" | "));
  SerialPrintU8(configPtr->bsPipe0Addr[1]); SerialPrintF(P(" | "));
  SerialPrintU8(configPtr->bsPipe0Addr[2]); SerialPrintF(P(" | "));
  SerialPrintU8(configPtr->bsPipe0Addr[3]); SerialPrintF(P(" | "));
  SerialPrintlnU8(configPtr->bsPipe0Addr[4]); 
  SerialPrintF(P("Pipe1 Addr: "));
  SerialPrintU8(configPtr->bsPipe1Addr[0]); SerialPrintF(P(" | "));
  SerialPrintU8(configPtr->bsPipe1Addr[1]); SerialPrintF(P(" | "));
  SerialPrintU8(configPtr->bsPipe1Addr[2]); SerialPrintF(P(" | "));
  SerialPrintU8(configPtr->bsPipe1Addr[3]); SerialPrintF(P(" | "));
  SerialPrintlnU8(configPtr->bsPipe1Addr[4]);
  SerialPrintF(P("Pipe2 : ")); SerialPrintU8(configPtr->pipe2to5Addr[0]);
  SerialPrintF(P(" Pipe3 : ")); SerialPrintU8(configPtr->pipe2to5Addr[1]); 
  SerialPrintF(P(" Pipe4 : ")); SerialPrintU8(configPtr->pipe2to5Addr[2]); 
  SerialPrintF(P(" Pipe5 : ")); SerialPrintlnU8(configPtr->pipe2to5Addr[3]);  
  // SerialPrintlnF(config.postUrl);
  SerialPrintF(P("Max Node: "));
  SerialPrintlnU8((uint8_t)configPtr->maxNode);
  SerialPrintF(P("Reserve Node: "));
  SerialPrintlnU8(configPtr->reserveNode);
  SerialPrintF(P("Max Tray Node: "));
  SerialPrintlnU8((uint8_t)configPtr->maxTrayNode);
  SerialPrintF(P("Check sum: "));
  SerialPrintlnU8(configPtr->checksum);
  SerialPrintF(P("Debug Off: "));
  SerialPrintlnU8((uint8_t)configPtr->isDebugOff);
  SerialPrintF(P("Registration: "));
  SerialPrintlnU8(configPtr->isSetupDone);
  SerialPrintlnF(P("----------------------"));
}

void printMqttF(struct mqtt_credentials *m)
{
    SerialPrintF(P("Server: ")); SerialPrintln(m->mqttServer);
    SerialPrintF(P("Client ID: ")); SerialPrintln(m->mqttClientId);
    SerialPrintF(P("User Name: ")); SerialPrintln(m->mqttUserName);
    SerialPrintF(P("Password: ")); SerialPrintln(m->mqttPassword);
    SerialPrintF(P("Payload Topic: ")); SerialPrintln(m->mqttPayloadtopic);
    SerialPrintF(P("Log Topic: ")); SerialPrintln(m->mqttConfigAcktopic);
    SerialPrintF(P("Last Will Topic: ")); SerialPrintln(m->mqttLastWilltopic);
    SerialPrintF(P("isConfigChanged: ")); SerialPrintlnU8(m->isConfigChanged);
    SerialPrintF(P("Checksum: ")); SerialPrintlnU8(m->checksum);
    SerialPrintF(P("Port: ")); SerialPrintlnU16(m->mqttPort);
    SerialPrintlnF(P("----------------------"));
}