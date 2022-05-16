/************************
 * This File contains the device id required to
 * initialize the device with unique identifier. 
 * 
 * RCMS_DEVICE_ID     used as DEVICE ID in the main code (must be a number)
 * RCMS_MQTT_ID       used as MQTT ID, PASSWORD AND CLIENT ID in the wifi and gsm section (must be string)
 * IOT_MQTT_SERVER    this is MQTT Server (url or ip allowed only. must be string)
 * IOT_MQTT_PORT      this is MQTT Port (must be a number)
 * 
 * *************************/

#ifndef _RCMS_CONF_H_
#define _RCMS_CONF_H_

#define RCMS_DEVICE_ID      3006
#define RCMS_MQTT_ID        "B3006"
#define IOT_MQTT_SERVER     "iotdev.pranisheba.com.bd"
#define IOT_MQTT_PORT        1884

#define SUBCRIPTION_ON  // to enable subscription


#endif //_RCMS_CONF_H_
