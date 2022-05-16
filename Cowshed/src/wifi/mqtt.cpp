#include "mqtt.h"


/*************** disconnect will message ***********/
const char disconnect_msg[] = "{\"status\" : 0}";
const char connected_msg[]  = "{\"status\" : 1}";

/**************Publish Topics**************************/
const char *payload_topic = BASE_STATION_ID"/data";
const char *log_topic      = BASE_STATION_ID"/log";
const char *config_ack_topic = BASE_STATION_ID"/conf_ack";
const char *will_feed = BASE_STATION_ID"/last_will";
/**************Subscribe Topics**************************/
const char *config_topic   = BASE_STATION_ID"/conf";

#define MQTT_CONNECT_MS 2000

//void set_pub_topics(void);
//void set_sub_topics(void);

bool statusFlag;
bool brokerConnectedFlag;

uint32_t mqttPrevMs;

// WiFiClient client;

WiFiClient espClient;
PubSubClient mqtt(espClient);
PubSubClient *pubSubPtr;


void mqttBegin()
{
  serverLedBegin();
  serverLedLow();
  debugSerial.print(F("Setting MQTT Topics.. "));
//  set_pub_topics();
//  set_sub_topics();
//  mqtt.will(will_feed, disconnect_msg);
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.connect(MQTT_CLIENTID,MQTT_USER,MQTT_PASS,will_feed,2,0,disconnect_msg);

  debugSerial.println(F("MQTT Setup Done"));
  mqttPrevMs = millis() + MQTT_CONNECT_MS;
}


bool mqttIsConnected()
{
  static uint32_t mqttLastConTime = 0;
  if(pubSubPtr->state() == MQTT_CONNECTED)
  {
    serverLedHigh();
    return true;
  }
  else
  {
    serverLedLow();
    return false;
  }
//  return mqtt.connected();
}
bool mqttConnect()
{
  static int mqttRetry = 4;
//  if (millis() - mqttPrevMs > MQTT_CONNECT_MS)
//  {
//    mqtt.disconnect();
  if(wifiIsconnected())
  {
    debugSerial.println(F("MQTT Connecting.."));
    if(mqttRetry > 2)
    {
      mqtt.disconnect();
      mqttRetry = 0;
      delay(1000);
    }
    mqtt.connect(MQTT_CLIENTID,MQTT_USER,MQTT_PASS,will_feed,2,0,disconnect_msg);
    delay(1000);
    bool mqttState = mqttIsConnected();
    if (mqttState)
    {
//      lastWill.publish(connected_msg);
      debugSerial.println(F("MQTT Connected"));
      serverLedHigh();
      return true;
    }
    else
    {
      debugSerial.println(F("MQTT NOT Connected"));
      serverLedLow();
      mqttRetry++;
    }
  }
  return false;
}

void mqtt_begin(void)
{
  debugSerial.print(F("Setting MQTT Topics.. "));
//  set_pub_topics();
//  set_sub_topics();
//
//  mqtt.will(will_feed, disconnect_msg);
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.connect(MQTT_CLIENTID,MQTT_USER,MQTT_PASS,will_feed,2,0,disconnect_msg);
  statusFlag = false;
  brokerConnectedFlag = false;
  debugSerial.println(F("Done"));
}
//void set_pub_topics(void)
//{
//  lastWill.set_topic(will_feed);
//  pubData.set_topic(payload_topic);
//  pubLog.set_topic(log_topic);
//  pubConfAck.set_topic(config_ack_topic);
//}
//
//void set_sub_topics(void)
//{
//  subConf.set_topic(config_topic);
//}


void mqtt_connect(void)
{
  int8_t connectStatus = (mqtt.state() == MQTT_CONNECTED);
  if (connectStatus)
  {
    if (statusFlag == false)
    {
//      lastWill.publish(connected_msg);
      statusFlag = true;
      // digitalWrite(SERVER_PIN, HIGH);
//      digitalWrite(LED_PIN, HIGH);
    }
    return;
  }
  else
  {
    // digitalWrite(SERVER_PIN, LOW);
    statusFlag = false;
  }
  int8_t retry = MQTT_CONNECT_MAX_TRY;
  while ((connectStatus = mqtt.connect(MQTT_CLIENTID,MQTT_USER,MQTT_PASS,will_feed,2,0,disconnect_msg)) != true && retry)
  {
    //.connect() will return zero if connected and it will not enters loop.
    debugSerial.print(F("MQTT Conn Status: ")); debugSerial.println(connectStatus);
//    debugSerial.println(mqtt.connectErrorString(connectStatus));
    mqtt.disconnect();
    delay(5000);
    retry--;
  }

}

void mqttConnectionLoop()
{
  int8_t status = mqtt.state();
  if (status)
  {
    if (brokerConnectedFlag == false)
    {
//      lastWill.publish(connected_msg);
      // digitalWrite(SERVER_PIN, HIGH);
      debugSerial.print(F("Connected"));
      brokerConnectedFlag = true;
    }
    return; //return function if connected
  }
  else
  {
    // digitalWrite(SERVER_PIN, LOW);
    debugSerial.print(F("\nMQTT Broker Connecting "));
    int8_t retry = MQTT_CONNECT_MAX_TRY;
    brokerConnectedFlag = false;
    do
    {
      debugSerial.print('.');
      // digitalToggle(SERVER_PIN);
      mqtt.disconnect();
      delay(200);
      status = mqtt.connect(MQTT_CLIENTID,MQTT_USER,MQTT_PASS,will_feed,2,0,disconnect_msg);              //connect will return 0 for connected
    } while (status != true && --retry);
  }
}
