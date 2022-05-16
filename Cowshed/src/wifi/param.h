#ifndef _PARAM_H_
#define _PARAM_H_
#include "../../conf.h"


/*******************WiFI Credentials*******************/
//#define WLAN_SSID       F("Shurjomukhi_Guest123")
//#define WLAN_PASS       F("myKeyis1")
//
//#define WLAN_SSID       F("Shurjomukhi_Dev")
//#define WLAN_PASS       F("Shurj#123#")

#define WLAN_SSID       F("praniSheba")
#define WLAN_PASS       F("#prani_sheba&")

/******************SMIOT MQTT Credentials*******************/
#define MQTT_SERVER     IOT_MQTT_SERVER
#define MQTT_PORT       IOT_MQTT_PORT
// #define MQTT_USER     "B3001"
// #define MQTT_PASS     "B3001"
// #define MQTT_CLIENTID "B3001"

#define MQTT_USER       RCMS_MQTT_ID
#define MQTT_PASS       RCMS_MQTT_ID
#define MQTT_CLIENTID   RCMS_MQTT_ID


/******************firmware version *******************/
#define FIRMWIRE_VERSION "3.0"

#define BASE_STATION_ID   MQTT_CLIENTID



// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

// Set serial for AT commands (to SIM800 module)
#if defined(ESP32_DEV)
  #define SerialAT Serial1
#else
  #define SerialAT Serial2
#endif



/******************Adafruit MQTT *******************/

//#define MQTT_SERVER   "io.adafruit.com"
//#define MQTT_PORT     1883
//#define MQTT_USER     "sshuvo93"
//#define MQTT_PASS      "aio_dkdl12DPPS8mJagCdBrrLsN7Vduu"
//#define MQTT_CLIENTID "B0001"

//#define AIO_SERVER      "io.adafruit.com"
//#define AIO_SERVERPORT  1883 
//#define AIO_USERNAME    "sshuvo93"
//#define AIO_KEY         "aio_dkdl12DPPS8mJagCdBrrLsN7Vduu"

/******************MQTT param************************/
/*******************Device Parameter*******************/



#define TOTAL_PUBLISHER 3
#define TOTAL_SUBSCRIBER 3
#define MQTT_CONNECT_MAX_TRY 3
#define MQTT_MAX_TOPIC_LEN 25
/******************PIN OUT****************************/


/**************Communication Speed******************/
#define DEBUG_SERIAL_SPEED  250000UL
#define PIPE_SERIAL_SPEED   57600UL
/**************RINGQ************/
#define RINGQ_BUFFER_LEN    4096 //byte

/***************Smart Config************/
#define SMART_CONFIG_TIMEOUT 2  /// timeout in min


#if defined(BOARD_MEGA328_V010)
  #define debugSerial Serial
#elif defined(ESP32_V010)
  #define debugSerial Serial
#elif defined(BOARD_MEGA1284_V010) && !defined(ESP32_V010)
  #define debugSerial Serial1
#else
  #error "No board is defined"
#endif

#endif
