#ifndef _UTILITY_H
#define _UTILITY_H
#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32_V010)
#include <esp_wifi.h>
#include <WiFi.h>
#endif


void wifiParamBegin();

void ntpBegin();
void ntpUpdateLoop();
uint32_t getUnixTime();
void printNtpTime();

void print_pointer(void *ptr);
char  *trim(char *str);
void digitalToggle(uint8_t pin );
#endif
