#include "sim800.h"


// Your GPRS credentials (leave empty, if not needed)
const char apn[]      = "gpinternet"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = "";

// HTTP UNIX credentials
const char http_server[]    = "showcase.api.linx.twenty57.net";
const char resource[]       = "/UnixTime/tounixtimestamp?datetime=now";
const int  port             = 80;

TinyGsm modem(SerialAT);

TinyGsmClient gsmClient(modem);

PubSubClient  simMqtt(gsmClient);

NewHttpClient    httpTimeClient(gsmClient, http_server, port);

uint32_t lastReconnectAttempt = 0;

uint32_t millisGprsUnixTime;
uint32_t gprsUnixTime;

void simSetup()
{
    // Set GSM module baud rate and UART pins
    // #if defined(ESP32_DEV)
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    // #else
    // Serial.println("Serial2 begin");
    // TinyGsmAutoBaud(SerialAT, 9600, 115200); //GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX
    // SerialAT.begin(38400, SERIAL_8N1, MODEM_RX, MODEM_TX);
    // #endif
    // Set modem reset, enable, power pins
#if defined(MODEM_PWKEY)
    pinMode(MODEM_PWKEY, OUTPUT);
    digitalWrite(MODEM_PWKEY, LOW);
#endif
#if defined(MODEM_RST)
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
#endif
#if defined(MODEM_POWER_ON)
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_POWER_ON, HIGH);
#endif
    
    
    delay(3000);
}

void simBegin()
{
    // Restart SIM800 module, it takes quite some time
    // To skip it, call init() instead of restart()
    // simSetup();
    // int read = digitalRead(MODEM_RST);
    // debugSerial.print("MODEM Reset key: ");
    // debugSerial.println(read);
    // #if defined(ESP32_DEV)
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    // #else
    // Serial.println("Serial2 begin");
    // TinyGsmAutoBaud(SerialAT, 9600, 115200); //GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX
    // // SerialAT.begin(38400, SERIAL_8N1, MODEM_RX, MODEM_TX);
    // #endif
    // delay(3000);
    delay(500);

    debugSerial.println("Initializing modem...");
    modem.restart();

    String modemInfo = modem.getModemInfo();
    debugSerial.print("Modem Info: ");
    debugSerial.println(modemInfo);
    delay(500); //10000
}

boolean simMqttConnect() 
{
    debugSerial.print("Connecting to ");
    //  debugSerial.print(broker);

    // Connect to MQTT Broker
    simMqtt.setServer(MQTT_SERVER, MQTT_PORT);
    boolean status = simMqtt.connect(MQTT_CLIENTID,MQTT_USER,MQTT_PASS,will_feed,2,0,disconnect_msg);

    //  boolean status = simMqtt.connect("GsmClientTest");

    // Or, if you want to authenticate MQTT:
    // boolean status = simMqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

    if (status == false) {
    debugSerial.println(" fail");
    return false;
    }

    debugSerial.println(" success");
    //  simMqtt.subscribe(topicLed);
    return simMqtt.connected();
}


bool isGprsAvailable()
{
    if (modem.isNetworkConnected()) 
    {
        debugSerial.println("Network connected");
        return true;
    }
    else
    {
       debugSerial.println("Network connection failed and waiting for network"); 
    }

    if (!modem.waitForNetwork(180000L, true)) 
    {
      debugSerial.println("Network connection failed");
    //   delay(10000);
      delay(1000);
    }
    if (modem.isNetworkConnected()) {
      debugSerial.println("Network connected");
      return true;
    }
    return false;
}



bool isGprsConnected()
{
    if (!modem.isGprsConnected()) 
    {
        debugSerial.println("GPRS disconnected!");
        debugSerial.print(F("Connecting to "));
        debugSerial.println(apn);
        if (!modem.gprsConnect(apn, gprsUser, gprsPass)) 
        {
            debugSerial.println(" fail");
            // delay(10000);
            delay(1000);
        }
        if (modem.isGprsConnected()) 
        { 
            debugSerial.println("GPRS reconnected"); 
            return true;
        }
    }
    else
    {
        debugSerial.println("GPRS connected!");
        return true;
    }
    return false;
}


uint32_t getGprsNtpTime()
{
    long time;
    int err = httpTimeClient.get(resource);
//  int err = httpTimeClient.get();
    if (err != 0) 
    {
        debugSerial.println(F("failed to connect"));
        // delay(10000);
        return 0UL;
    }

    int status = httpTimeClient.responseStatusCode();
    debugSerial.print(F("[HTTP UNIX] Response status code: "));
    debugSerial.println(status);
    if (!status) 
    {
        // delay(10000);
        return 0UL;
    }

    while (httpTimeClient.headerAvailable()) 
    {
        String headerName  = httpTimeClient.readHeaderName();
        String headerValue = httpTimeClient.readHeaderValue();
    //    debugSerial.println("    " + headerName + " : " + headerValue);
    }

    if(status == 200)
    {
        int length = httpTimeClient.contentLength();
        if (length >= 0) 
        {
        //    debugSerial.print(F("Content length is: "));
        //    debugSerial.println(length);
            String body = httpTimeClient.responseBody();
            // Converting UNIXTime Json to int    
            // DynamicJsonDocument jsonData(1024);
            // deserializeJson(jsonData, body);    // extract data from string to json format
            // JsonObject obj = jsonData.as<JsonObject>();
            debugSerial.println(body);
            int index1 = body.indexOf(':');
            index1 = body.indexOf('\"',index1);
            int lasIndex = body.lastIndexOf('\"');
            String timeS = body.substring(index1+1,lasIndex);
            gprsUnixTime = (uint32_t)timeS.toInt();
            gprsUnixTime +=5;
            time = gprsUnixTime;
            millisGprsUnixTime = (uint32_t)millis();
            debugSerial.print(F("GPRS UnixTimeStamp: "));
            debugSerial.println((uint32_t)time );
        }
        if (httpTimeClient.isResponseChunked()) 
        {
            debugSerial.println(F("The response is chunked"));
        }
    }
  // Shutdown
  httpTimeClient.stop();  
//   debugSerial.println(F("Server disconnected"));

    // return (uint32_t)1630000UL;
    return (uint32_t)time;
}


uint32_t getGprsUnixTime()
{
    uint32_t nowTime;
    nowTime = millis();
    uint32_t offset = (nowTime - millisGprsUnixTime)/1000;
    return (gprsUnixTime + offset);
}

bool gsmRadioOff()
{
    return modem.radioOff();
}

bool gsmPowerOff()
{
    return modem.poweroff();
}
