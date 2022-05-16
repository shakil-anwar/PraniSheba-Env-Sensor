#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "ota.h"
#include "../../../avr/device_ps/Sensors.h"

// const char* ssid = "APS_Office";  // Enter SSID here
// const char* password = "praniSheba@2021#";  //Enter Password here

// ESP8266WebServer server(80);
bool startCalibServer();
void handle_OnConnect();
void handle_NotFound();
void handleData();
void handleChkFirmware();
void handleUpdateFirmware();
float calibMethane();
float calibAmmonia();

String SendHTML(float METHANE, float AMMONIA, float CO2, float HUMIDITY, bool autoRefresh);
String sendCalibrationPage(float methaneR0, float ammoniaR0);
String displayValues(float methaneR0, float ammoniaR0);
String sendNewFwPage(String fwVersion);
void runCalibServer();

extern WebServer server;


#endif
