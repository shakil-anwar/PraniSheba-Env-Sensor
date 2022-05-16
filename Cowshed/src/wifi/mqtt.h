#ifndef _MQTT_H_
#define _MQTT_H_
#include "wifi.h"
#include "param.h"
#include "utility.h"
#include "pin.h"

//#include "Adafruit_MQTT.h"
//#include "Adafruit_MQTT_Client.h"



#if defined(PROD_BUILD)
#include "./lib/pubsubclient/src/PubSubClient.h"
#else
#include "PubSubClient.h"
#endif

#include "sim800.h"


/**********************New API*********************/
#define serverLedBegin() pinMode(SERVER_PIN, OUTPUT);
#define serverLedLow()   digitalWrite(SERVER_PIN,LOW);
#define serverLedHigh()  digitalWrite(SERVER_PIN, HIGH);
void mqttBegin();
bool mqttIsConnected();
bool mqttConnect();
/**********************Old API*********************/
void mqtt_begin(void);
void mqtt_connect(void);
void mqttConnectionLoop();
void mqtt_test_send_all();
void mqttPrint(char *str);

//extern Adafruit_MQTT_Publish pubData;
//extern Adafruit_MQTT_Publish pubLog;
//extern Adafruit_MQTT_Publish pubConfAck;
//
//extern Adafruit_MQTT_Client mqtt;
extern PubSubClient mqtt;
extern const char *payload_topic;
extern const char *log_topic;
extern const char *config_ack_topic;
extern const char *will_feed;
extern const char *config_topic;

/*************** disconnect will message ***********/
extern const char disconnect_msg[];
extern const char connected_msg[];

extern PubSubClient *pubSubPtr;

#endif
