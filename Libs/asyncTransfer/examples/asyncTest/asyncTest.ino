#include "asyncServer.h"
#include "MemQ.h"



typedef struct sensor_t
{
  float current;
  float voltage;
};

typedef union payload_t
{
  sensor_t sensor;
};

char *toJson(uint8_t *payload, char *jsonBuf, uint8_t total = 1);
void sendSerial(const char *data);
int wait();

MemQ memQ(0,100);
AsyncServer server(&memQ);
String rcvStr;

bool permitFlag;
void setup() 
{
  Serial.begin(9600);
  server.setServerCbs(sendSerial,wait);
  server.setSchema(sizeof(sensor_t),1);
  server.setJson(toJson,256);
  server.start();
  rcvStr.reserve(32);
  
  permitFlag = true;
  Serial.println(F("Setup done"));
}

void loop() 
{
  server.sendLoop(permitFlag);
}

void sendSerial(const char *data)
{
  Serial.println(data);
}
int wait()
{
  if(Serial.available())
  {
    rcvStr = Serial.readString();
    int retData = rcvStr.toInt();
    Serial.print(F("ACK Received: "));Serial.println(retData);
  }
}

char *toJson(uint8_t *payload, char *jsonBuf, uint8_t total)
{
  Serial.println(F("Converting JSON"));
  return jsonBuf;
}
