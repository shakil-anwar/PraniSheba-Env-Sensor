#ifndef _WIFI_H_
#define _WIFI_H_
#include "Arduino.h"
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32_V010)
#include <esp_wifi.h>
// #include <WiFi.h>
#include <WiFiClientSecure.h>
#endif
#include "pin.h"
#include "utility.h"
#include "../common_lib/iotStructure/iotStructure.h"





/**********************New API*********************/
#if defined(ESP32_V010)
#define wifiLedBegin() ({\
                        debugSerial.println("WiFiLED Begin");\
                        })
#define wifiLedLow()   ({\
                        debugSerial.println("WiFiLED LOW");\
                        })
                        
#define wifiLedHigh()  ({\
                        debugSerial.println("WiFiLED HIGH");\
                        })
                        
#define wifiLedToggle()({\
                        debugSerial.print(".");\
                        })
#else
#define wifiLedBegin() ({\
                        pinMode(WIFI_LED, OUTPUT);\
                        })
#define wifiLedLow()   ({\
                        digitalWrite(WIFI_LED,LOW);\
                        })
                        
#define wifiLedHigh()  ({\
                        digitalWrite(WIFI_LED,HIGH);\
                        })
                        
#define wifiLedToggle()({\
                        digitalToggle(WIFI_LED);\
                        })
#endif
// void wifiBegin();
void wifiBegin(String ssid, String pass);
bool wifiIsconnected();
bool wifiConnect();
String getSSID();

/**********************Old API*********************/
void wifi_connect(void);
void tcp_print(char *str);



#endif
