#include "wifi.h"
#include "param.h"
// #include "./lib/WiFiManager/softApClass.h"


#define WIFI_LED_BLINK_MS   500

uint32_t wifiPrevMs;
uint16_t wifiConnCounter;
// softApClass ap;

/**************For TCP Client ******************/
//Server parameter
// int             serverPort  = 8000;
// IPAddress       serverIP(192, 168, 0, 105);

// WiFiClient      tcpClient;

/************************************************/



void wifiBegin(String ssid, String pass)
{
  wifiLedBegin();
  wifiLedLow();
//  WiFi.begin(WLAN_SSID, WLAN_PASS);
  // WiFi.begin();
  if(ssid.length() && pass.length())
  {
#if defined(ESP32_V010)
  // char wifiName[ssid.length()];
  // char wifiPass[pass.length()];
  // ssid.toCharArray(wifiName, ssid.length());
  // pass.toCharArray(wifiPass, pass.length());
  // WiFi.begin(wifiName, wifiPass);
  WiFi.begin(ssid.c_str(),pass.c_str());
#else
  WiFi.begin(ssid, pass);
#endif
  }
  else
  {
#if defined(ESP32_V010)
    debugSerial.print("SSID: ");debugSerial.println(getSSID());
    debugSerial.print("PSK: ");debugSerial.println(WiFi.psk());
    WiFi.begin(getSSID().c_str(), WiFi.psk().c_str());
#else
    WiFi.begin();
#endif
  }


  debugSerial.println("Wifi Setup Done");

  wifiPrevMs = millis();
  wifiConnCounter = 0;
}

bool wifiConnect()
{
  uint32_t nowMillis = millis();

  if( nowMillis - wifiPrevMs>WIFI_LED_BLINK_MS)
  {
    if(wifiConnCounter == 0)
    {
      debugSerial.println(F("WiFi Connect "));
    }
    wifiConnCounter++;
    wifiLedToggle();
    debugSerial.print(".");
    if(wifiConnCounter %40 == 0)
    {
      debugSerial.println();
    }
    
    wifiPrevMs = nowMillis;

    if(WiFi.status() == WL_CONNECTED)
    {
      wifiConnCounter = 0;
      // wifiLedHigh();
    //    wifiLedHigh();
      debugSerial.println(F("\r\nWiFi Connected"));
      return true;
    }
    else
    {
      // wifiLedLow();
    //    delay(500);
      return false;
    }
  }
  return false;
}

bool wifiIsconnected()
{
  return (WiFi.status() == WL_CONNECTED);
}


void wifi_connect(void)
{
  debugSerial.print(F("WiFi Connecting"));
  // pinMode(LED_PIN, OUTPUT);
  // pinMode(WIFI_LED, OUTPUT);
  // digitalWrite(WIFI_LED, LOW);

#if defined(ESP8266)
  WiFi.begin(WLAN_SSID, WLAN_PASS);
#elif defined(ESP32_V010)
  WiFi.begin((const char*)WLAN_SSID, (const char*)WLAN_PASS);
#endif

  while (WiFi.status() != WL_CONNECTED)
  {
    // digitalToggle(WIFI_LED);
//    digitalToggle(LED_PIN);
    delay(200);
    debugSerial.print(".");
  }
  // digitalWrite(WIFI_LED, HIGH);
//  digitalWrite(LED_PIN, HIGH);
  debugSerial.println(F("connected"));
}


// void tcp_print(char *str)
// {
//   if (tcpClient.connect(serverIP, serverPort))
//   {
//     tcpClient.write(str);
//   }
//   else
//   {
//     debugSerial.println(F("client connection error"));
//   }
// }

#if defined(ESP32_V010)

String getSSID()  //This function has been added for using instead of default WiFi.SSID() as it returns null after a restarts(get info from wifi_ap_record_t structure)
{
    wifi_ap_record_t info;
    if(!esp_wifi_sta_get_ap_info(&info)) {
        return String(reinterpret_cast<char*>(info.ssid));
    }
    else{
          wifi_config_t conf;
          esp_wifi_get_config(WIFI_IF_STA, &conf);
          return String(reinterpret_cast<char*>(conf.sta.ssid));    
    }   //This block has been added manually
}
#endif


