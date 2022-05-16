#ifndef __IOT__STRUCTURE__H__
#define __IOT__STRUCTURE__H__

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// #include "./iotStructure/iotStructure.h"

/***************************Device Type opcode*******************************/
#define BOLUS_TYPE                  '1'
#define BOLUS_TEMP_TYPE             '2'
#define BOLUS_GYRO_TYPE             '3'
#define BOLUS_ID_PREFIX             'S'

#define GASSENSOR_TYPE              11
#define GASSENSOR_LOG_TYPE          23
#define GAS_ID_PREFIX               'G'

#define BASE_STATION_LOG_TYPE       22
#define BASE_STATION_PREFIX         'B'

/**************************Bolus specific Parameters***********************/
#define BOLUS_SAMPLE_IN_PACKET      8    //Total Samples in a packet
#define BOLUS_SAMPLE_INTERVAL       500 //interval in milisecond
#define GYRO_SAMPLE_INTERVAL        2500  // interval in milisecond
#define GYRO_SAMPLE_IN_PACKET       3
#define BOLUS_Temp_SAMPLE_IN_PACKET 4



/**************************************Payload Schema*****************************/

/********************New data structure****************/
struct header_t
{
  uint8_t type;
  uint8_t checksum;
  uint16_t id;
}__attribute__ ((packed));


typedef struct bsParam_t
{
	uint16_t bsId;
	uint32_t bsStartingTime;
	uint32_t bsServerRegTime;
	uint8_t deviceType;
	uint8_t subscriptionStatus;	
};

struct bolusXYZ_t
{
  //Header
  struct header_t header;
  //payload
  unsigned long unixTime;
  uint8_t x[BOLUS_SAMPLE_IN_PACKET];
  uint8_t y[BOLUS_SAMPLE_IN_PACKET];
  uint8_t z[BOLUS_SAMPLE_IN_PACKET];
}__attribute__ ((packed));


struct bolusGyroPacket_t
{
  uint16_t gyData[3];
  uint16_t accData[3];
}__attribute__ ((packed));

struct bolusGyro_t
{
  //Header
  struct header_t header;     //4 byte
  unsigned long unixTime;
  //gyro 12 byte packet
  struct bolusGyroPacket_t gyro[2];
}__attribute__ ((packed));

struct bolusTempPacket_t
{
  unsigned long unixTime;
  uint16_t value;
}__attribute__ ((packed));
struct bolusTemp_t
{
  //Header
  struct header_t header;     //4 byte

  //payload 6 byte
  struct bolusTempPacket_t temp[4];//24 byte
  uint16_t batVolt;           //2 byte
  uint16_t errorCode;         //2 byte
};

struct gasSensor_t
{
  //header
  struct header_t header;
  //payload
  unsigned long unixTime;
  float ammonia;
  float methane;
#if defined(DEVICE_HAS_SMOKE_SENSOR)
  float smoke;
#endif
  float hum;
  float temp;
  uint16_t logicVolt;
  uint16_t errorCode;
#if !defined(DEVICE_HAS_SMOKE_SENSOR)
  uint8_t isServerSynced;
  uint8_t reserve[3];
#endif
}__attribute__ ((packed));

struct gasSensorLog_t
{
  struct header_t header;
  uint8_t errorCode;
  uint8_t hardwareErrorCode;
  uint16_t restartCount;
  uint16_t slotMissed;
  uint16_t samplingFreq;
  float railVoltage;
  unsigned long unixTime;
  unsigned long flashAvailablePackets;
}__attribute__ ((packed));

struct gasSensorMetaLog_t
{
  uint16_t restartCount;
  uint32_t lastUnixTime;
  uint8_t reserve;
  uint8_t checksum;
}__attribute__ ((packed));

struct bslog_t
{
  struct header_t header;
  bool isNetOk;
  uint8_t errorCode;
  uint8_t hardwareErrorCode;
  uint8_t radioStatus;
  uint8_t syncType;
  uint16_t restartCount;
  uint16_t totalSlotTime;
  uint16_t clientConnectFail;
  unsigned long flashAvailablePackets;
  unsigned long unixTime;
  float railVoltage;
}__attribute__ ((packed));

typedef union payload_t
{
  struct bolusXYZ_t bolus;
  struct gasSensor_t gasSensor;
}payload_t;



typedef struct bsConfig_t
{
  // uint16_t sendInterval;
  uint16_t bsId;
  uint16_t  momentDuration;  //lenthan 65535
  uint8_t bsPipe0Addr[5];
  uint8_t bsPipe1Addr[5]; // this is the pipe 1 address and base station address
  uint8_t pipe2to5Addr[4];
  uint8_t maxNode;  // less than 255
  uint8_t reserveNode;  //less than 255
  uint8_t maxTrayNode;
  uint8_t checksum;
  uint8_t isSetupDone;
  uint8_t deviceType;
  bool isDebugOff;
}bsConfig_t;


typedef struct mqtt_credentials
{
  char mqttServer[32];
  char mqttClientId[10];
  char mqttUserName[10];
  char mqttPassword[10];
  char mqttPayloadtopic[16];
  char mqttLogtopic[16];
  char mqttConfigAcktopic[16];
  char mqttLastWilltopic[16];
  bool isConfigChanged;
  uint8_t checksum;
  uint16_t mqttPort;
}mqtt_credentials;

typedef struct espLog
{
  struct header_t header;
  bool isWifiAvailable;
  bool isGsmAvailable;
  uint8_t isInternetAvailable;
  uint8_t mqttAvailablePacket;
  uint8_t ringQavailablePacket;
  uint8_t espErrorCode;
  uint32_t lastAvrCommunicationTime;
  uint32_t lastNetAvailableTime;
  uint32_t unixTime;
}espLog;


typedef enum networkType_t
{
  WIFI_STATE = 0x01,
  GSM_STATE = 0x02,
}networkType_t;

typedef struct netState_t
{
  enum networkType_t networkType;  //1=WIFI_ONLY, 2=GSM_ONLY, 3= WIFI_GSM 
  bool deviceHasWifi;
  bool deviceHasGsm;  
  bool wifiCheckFlag;
  bool networkAvail;
  bool internetAvail;
  uint8_t wifiAvail;
  uint8_t  mqttStatus;
  uint32_t ntpTime;
  uint8_t dataSendReady;
}netState_t;

struct subscriptionParam_t
{
  struct header_t header;
  uint16_t isDeviceHasSubscription;
  uint16_t isSubscriptionOn;
  uint16_t remainingDays;
  uint16_t reserve[3];
}__attribute__((packed));

#if defined(BOARD_MEGA1284_V010)
#define MAX_SERIAL_PACKET   10
#define MQTT_ACK_WAIT_TIME  5  //seconds
#define DATA_SEND_SUCCESS   2
#define DATA_SEND_FAILED    1


typedef struct pipeData_t
{
    unsigned long flashAddr;
    unsigned long sendTime;
    uint32_t laskAckTime;
    uint8_t mgsId;
    int8_t mgsAck;
    int8_t sendFlag;
}pipeData_t;

typedef struct pipeSendParam_t
{
    pipeData_t pipeData[MAX_SERIAL_PACKET];
    int currentId;
    uint8_t* dataPtr;
}pipeSendParam_t;

extern pipeSendParam_t pipeParam;
#endif



void printBolus(struct bolusXYZ_t *bolus);
void printGasSensor(struct gasSensor_t *gas);
void printBsLogData(struct bslog_t *bsLogPtr);
void configPrint(bsConfig_t *bootPtr);
void printMqttF(struct mqtt_credentials *m);

extern struct netState_t netState;
extern struct bsParam_t bsParam;
extern struct subscriptionParam_t subscribeParam;

#ifdef __cplusplus 
}
#endif

#endif
