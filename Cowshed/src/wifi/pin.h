#ifndef _PIN_H_
#define _PIN_H_
#include <Arduino.h>
#include "param.h"


#if defined(ESP32_V010)

  #define SERVER_PIN  200
  #define WIFI_LED    201
  #define LED_PIN     202

  #if defined(ESP32_DEV)
    #define MODEM_RST            5
    #define MODEM_PWKEY          4
    #define MODEM_POWER_ON       23
    #define MODEM_TX             27
    #define MODEM_RX             26
  #else
    #warning "<---------TEST BOARD--------->"
    #define MODEM_RST            13
#if defined(GSM_V010)
    #define MODEM_TX             17
    #define MODEM_RX             16
#else
    #define MODEM_TX             14
    #define MODEM_RX             12
#endif
  #endif
#else
  #error "No board is defined"
#endif
#endif
