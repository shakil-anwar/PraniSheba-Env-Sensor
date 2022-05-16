#include "softApClass.h"



#define BS_FLAG_ADDRESS 0
#define MQTT_FLAG_ADDRESS 2
#define ESP_BS_ADDRESS 4
#define ESP_MQTT_ADDRESS 200


mqtt_credentials serverMqtt = {"", "", "", "", "", "", "", "", 0, 0, 0};
bsConfig_t bsConf = {0, 0, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0};

bool mqttflag = false, bsflag = false;





bool softApClass::startConfigPortal(char const *apName, char const *apPass)
{
  configPortalActive = true;
  _configPortalStart = millis();
  _apName = apName;
  _apPass = apPass;
  #ifdef DEBUG
  debugSerial.print(F("apName is: "));debugSerial.println(apName);
  debugSerial.print(F("apPass is: "));debugSerial.println(apPass);
  #endif
  // WiFi.enableSTA(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apName, apPass);
  delay(2000);


  setupConfigPortal();
  setupDNSD();
  debugSerial.println(F("Config Portal Running, waiting for clients..."));
  if(_configPortalTimeout > 0) 
  {
    #ifdef DEBUG
    debugSerial.print(F("Portal timeout in "));debugSerial.print((String)(_configPortalTimeout/1000)); debugSerial.println(F(" seconds"));
    #endif
  }
  return true;
}


bool softApClass::startConfigPortal(char const *apName)
{
  startConfigPortal(apName, NULL);
  return true;
}

void softApClass::setupConfigPortal()
{

  server.reset(new WM_WebServer(_httpPort));
  if(_webservercallback != NULL)
  {
    _webservercallback();
  }

  server->on(String(F("/")).c_str(),                std::bind(&softApClass::handleRoot, this));
  server->on(String(F("/devlogin")).c_str(),        std::bind(&softApClass::handleDevLogin, this));
  server->on(String(F("/devloginsubmit")).c_str(),  std::bind(&softApClass::handleDevLoginSubmit, this));
  server->on(String(F("/wifi")).c_str(),            std::bind(&softApClass::handleWifi, this, true));
  server->on(String(F("/0wifi")).c_str(),           std::bind(&softApClass::handleWifi, this, false));
  server->on(String(F("/wifisave")).c_str(),        std::bind(&softApClass::handleWifiSave, this));
  server->on(String(F("/home")).c_str(),            std::bind(&softApClass::httpHome, this));

  server->on(String(F("/bsconfig")).c_str(),        std::bind(&softApClass::handleBSConfig, this));
  server->on(String(F("/bsinfopreview")).c_str(),   std::bind(&softApClass::previewBSInfo, this));
  server->on(String(F("/bsinfosubmit")).c_str(),    std::bind(&softApClass::saveBSInfo, this));


  server->on(String(F("/mqttconf")).c_str(),        std::bind(&softApClass::handleMqttConfig, this));
  server->on(String(F("/mqttinfopreview")).c_str(), std::bind(&softApClass::previewMqttInfo, this));
  server->on(String(F("/mqttinfosubmit")).c_str(),  std::bind(&softApClass::saveMqttInfo, this));


  server->on(String(F("/i")).c_str(),               std::bind(&softApClass::handleInfo, this));
  server->on(String(F("/r")).c_str(),               std::bind(&softApClass::handleReset, this));


  // server->onNotFound (std::bind(&softApClass::handleNotFound, this));



  server->begin();
  #ifdef DEBUG
  debugSerial.println("HTTP server started");
  #endif
}

bool softApClass::configPortalHasTimeout()
{
  uint16_t clientLogInterval = 30000;

  if(!configPortalActive) return true;

  if(_configPortalTimeout == 0 || (WiFi_softap_num_stations() > 0))
  {
    if(millis() - timer > clientLogInterval)
    {
      timer = millis();
      #ifdef DEBUG
      debugSerial.print("Number of client connected: ");debugSerial.println((String)WiFi_softap_num_stations());
      #endif
    }
    _configPortalStart = millis();
    return false;
  }

  if (millis() > _configPortalStart + _configPortalTimeout)
  {
    //debugSerial.println("Portal has timed out");
    return true;
  }
  else if (_debug && ((millis() - timer) > clientLogInterval))
  {
    timer = millis();
    #ifdef DEBUG
    debugSerial.print("Portal Timeout in ");debugSerial.print((_configPortalStart+_configPortalTimeout - millis()) / 1000);debugSerial.println(" Seconds");
    #endif
  }


  return false;

  
}

uint8_t softApClass::handleConfigPortal()
{

  uint8_t state;

  // debugSerial.print("configPortalActive: ");debugSerial.println(configPortalActive);
  if(configPortalActive)
  {
    dnsServer->processNextRequest();
  }
  // debugSerial.println("configPortalHasTimeout below");
  if(configPortalHasTimeout())
  {
    // debugSerial.println("configPortalHasTimeout now");
    #ifdef DEBUG
    debugSerial.println("Loop Abort");
    #endif
    shutdownConfigPortal();
    
    return false;
  }
  state = processConfigPortal();
  // debugSerial.print("state: ");debugSerial.println(state);

  if(state != WL_IDLE_STATUS)
  {
    //result = (state == WL_CONNECTED);
    #ifdef DEBUG
    debugSerial.println(F("configPortal loop break"));
    #endif
    return false;
  }

  if(!configPortalActive)
  {
    #ifdef DEBUG
    debugSerial.print(F("Config Portal Active: "));debugSerial.println(configPortalActive);
    #endif
    return false;
  }





  yield();
  return true;
}





uint8_t softApClass::processConfigPortal()
{

  
  if(configPortalActive)
  {
    dnsServer->processNextRequest();
  }
  server->handleClient();

  if(connect)
  {
    connect = false;
    #ifdef DEBUG
    debugSerial.println(F("Processing save"));
    #endif
    if(_enableCaptivePortal) delay(_cpclosedelay);

    if(_ssid == "")
    {
      #ifdef DEBUG
      debugSerial.println(F("NO ssid, skipping wifi save"));
      #endif
    }
    else
    {
      uint8_t res = connectWifi(_ssid, _pass, _connectonsave) == WL_CONNECTED;
      if(res || (!_connectonsave))
      {
        // if(_connectonsave)
        // {
        //   debugSerial.println(F("SAVED with no connect to new AP"));
        // }
        // else
        // {
          #ifdef DEBUG
          debugSerial.println(F("Connect to new AP [SUCCESS]"));
          debugSerial.print(F("Got IP Address: "));debugSerial.println(WiFi.localIP());
          #endif

        // }

        if(_savewificallback != NULL)
        {
          _savewificallback();
        }
        shutdownConfigPortal();

        if(!_connectonsave) return WL_IDLE_STATUS;
        return WL_CONNECTED;
      }

      #ifdef DEBUG
      debugSerial.println(F("[ERROR] Connect to new AP Failed"));
      #endif
      _ssid = "";
      _pass = "";
      WiFi_Disconnect();
      WiFi_enableSTA(false);
      #ifdef DEBUG
      debugSerial.println(F("Disabling STA"));
      #endif

    }

  }

  return WL_IDLE_STATUS;


}








bool softApClass::captivePortal()
{
  #ifdef DEBUG
  debugSerial.print("hostHeader->");debugSerial.println(server->hostHeader());
  #endif

  if(!_enableCaptivePortal) return false;

  String serverLoc = toStringIp(server->client().localIP());
  #ifdef DEBUG
  debugSerial.print("serverLoc: ");debugSerial.println(serverLoc);
  #endif
  bool doredirect = serverLoc != server->hostHeader(); 
  debugSerial.print(F("doredirect: "));debugSerial.println(doredirect);
  if(doredirect)
  {
    #ifdef DEBUG
    debugSerial.println("<- Request redirected to captive portal");
    #endif
    server->sendHeader(F("Location"), (String)F("http://") + serverLoc, true);
    server->send(302, F("text/plain"), "");
    server->client().stop();

    return true;
  }
  return false;


}

void softApClass::setupDNSD()
{
  dnsServer.reset(new DNSServer());
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  #ifdef DEBUG
  debugSerial.print("dns server started with ip: ");debugSerial.println(WiFi.softAPIP());
  #endif
  dnsServer->start(DNS_PORT, F("*"), WiFi.softAPIP());

}

void softApClass::handleRequest() 
{
  
  bool testauth = false;
  if(!testauth) return;
  #ifdef DEBUG
  debugSerial.println(F("DOING AUTH"));
  #endif
  // bool res = server->authenticate(_httpUserName, _httpPassWord);
  bool res = server->authenticate("admin", "12345");
  if(!res)
  {
    
    server->requestAuthentication(HTTPAuthMethod::BASIC_AUTH); // DIGEST_AUTH
    #ifdef DEBUG
    debugSerial.println(F("AUTH FAIL"));
    #endif
  }
}


uint8_t softApClass::connectWifi(String ssid, String pass, bool connect)
{
  #ifdef DEBUG
  debugSerial.println(F("Connecting as wifi client..."));
  #endif
  uint8_t retry = 1;
  uint8_t connRes = (uint8_t)WL_NO_SSID_AVAIL;
  WiFi_Disconnect(); // To increase stability

  while(retry <= _connectRetries && (connRes !=  WL_CONNECTED))
  {
    if(_connectRetries > 1)
    {
      #ifdef DEBUG
      debugSerial.print(F("Connect WiFi, Attempt #"));debugSerial.print((String)retry);
      debugSerial.print(F(" of "));debugSerial.println((String)_connectRetries);
      #endif

    }
  
    if (ssid != "")
    {
      wifiConnectNew(ssid, pass, connect);
      if(_saveTimeout > 0)
      {
        connRes = waitForConnectResult(_saveTimeout);

      }
      else
      {
        connRes = waitForConnectResult(0);
      }
      
    }
    else
    {
      // if(WiFi_hasAutoConnect())
      // {
      //   wifiConnectDefault();
      //   connRes = waitForConnectResult();
      // }
      // else

      //Upper lines are off to avoid autoconnect
      {
        #ifdef DEBUG
        debugSerial.println(F("No wifi saved, skipping"));
        #endif
      }
    }
    #ifdef DEBUG
    debugSerial.print(F("Connection result: "));debugSerial.println(getWLStatusString(connRes));
    #endif
    retry++;
  }

  return connRes;
}

bool softApClass::wifiConnectNew(String ssid, String pass, bool connect)
{
  bool ret = false;
  #ifdef DEBUG
  debugSerial.print("Connecting to NEW AP: ");debugSerial.println(ssid);
  debugSerial.print("Using Password: ");debugSerial.println(pass);
  #endif
  WiFi_enableSTA(true, storeSTAmode);
  WiFi.persistent(true);
  
  ret = WiFi.begin(ssid.c_str(), pass.c_str(), 0, NULL, connect);
  WiFi.persistent(false);
  #ifdef DEBUG
  if(!ret) debugSerial.println(F("[ERROR] wifi begin failed"));
  #endif
  return ret;
}








bool softApClass::WiFi_enableSTA(bool enable, bool persistent)
{
  #ifdef DEBUG
  debugSerial.print(F("WiFi_enableSTA: "));debugSerial.println((String) enable? "enable" : "disable");
  #endif

  #ifdef ESP8266
    WiFiMode_t newMode;
    WiFiMode_t currentMode = WiFi.getMode();
    bool isEnabled = (currentMode & WIFI_STA) != 0;
    if(enable) newMode = (WiFiMode_t) (currentMode | WIFI_STA);
    else newMode = (WiFiMode_t) (currentMode & (~WIFI_STA));

    if((isEnabled != enable) || persistent)
    {
      if(enable)
      {
        #ifdef DEBUG
        if(persistent) debugSerial.println(F("enableSTA persistent ON"));
        #endif
        return WiFi_Mode(newMode, persistent);
      }
      else
      {
        return WiFi_Mode(newMode, persistent);
      }
    }
    else
    {
      return true;
    }

  #elif defined(ESP32_V010)
    bool ret;
    if(persistent && esp32persistent) WiFi.persistent(true);
    ret = WiFi.enableSTA(enable);
    if(persistent && esp32persistent) WiFi.persistent(false);
    return ret;
  #endif

}

bool softApClass::WiFi_enableSTA(bool enable)
{
  return WiFi_enableSTA(enable, false);
}






uint8_t softApClass::waitForConnectResult(uint32_t timeout)
{

  if(timeout == 0)
  {
    #ifdef DEBUG
    debugSerial.println(F("connectTimeout not set, ESP waitForConnectResult..."));
    #endif
    return WiFi.waitForConnectResult();
  }

  unsigned long timeoutmillis = millis() + timeout;
  #ifdef DEBUG
  debugSerial.print(timeout);debugSerial.println(F("ms timeout, waiting for connect..."));
  #endif

  uint8_t status = WiFi.status();

  while(millis() < timeoutmillis)
  {
    status = WiFi.status();
    if(status == WL_CONNECTED || status == WL_CONNECT_FAILED)
    {
      return status;
    }
    #ifdef DEBUG
    debugSerial.print(F("."));
    #endif
    delay(100);
  }

  return status;

}


uint8_t softApClass::waitForConnectResult()
{
  if(_connectTimeout > 0)
  {
    #ifdef DEBUG
    debugSerial.print(_connectTimeout);debugSerial.println(F("ms connectTimeout set"));
    #endif

  }
  return waitForConnectResult(_connectTimeout);
}





bool softApClass::WiFi_Disconnect()
{
  #ifdef ESP8266
    if((WiFi.getMode() & WIFI_STA) != 0)
    {
      bool ret;
      #ifdef DEBUG
      debugSerial.println(F("WiFi Station disconnect"));
      #endif
      ETS_UART_INTR_DISABLE();
      ret = wifi_station_disconnect();
      ETS_UART_INTR_ENABLE();
      return ret;
    }

  #elif defined(ESP32_V010)
    #ifdef DEBUG
    debugSerial.println(F("WiFi Station disconnect"));
    #endif
    return WiFi.disconnect();
  #endif
  return false;
}





bool softApClass::WiFi_hasAutoConnect()
{
  return WiFi_SSID(true) != "";
}



String softApClass::WiFi_SSID(bool persistent) const
{

    #ifdef ESP8266
    struct station_config conf;
    if(persistent) wifi_station_get_config_default(&conf);
    else wifi_station_get_config(&conf);

    char tmp[33]; //ssid can be up to 32chars, => plus null term
    memcpy(tmp, conf.ssid, sizeof(conf.ssid));
    tmp[32] = 0; //nullterm in case of 32 char ssid
    return String(reinterpret_cast<char*>(tmp));
    
    #elif defined(ESP32_V010)
    if(persistent)
    {
      wifi_config_t conf;
      esp_wifi_get_config(WIFI_IF_STA, &conf);
      return String(reinterpret_cast<const char*>(conf.sta.ssid));
    }
    else
    {
      if(WiFiGenericClass::getMode() == WIFI_MODE_NULL)
      {
        return String();
      }
      wifi_ap_record_t info;
      if(!esp_wifi_sta_get_ap_info(&info)) 
      {
        return String(reinterpret_cast<char*>(info.ssid));
      }
      return String();
    }
    #endif
}


String softApClass::WiFi_psk(bool persistent) const 
{
    #ifdef ESP8266
    struct station_config conf;

    if(persistent) wifi_station_get_config_default(&conf);
    else wifi_station_get_config(&conf);

    char tmp[65]; //psk is 64 bytes hex => plus null term
    memcpy(tmp, conf.password, sizeof(conf.password));
    tmp[64] = 0; //null term in case of 64 byte psk
    return String(reinterpret_cast<char*>(tmp));
    
    #elif defined(ESP32_V010)
    // only if wifi is init
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL)
    {
      return String();
    }
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    return String(reinterpret_cast<char*>(conf.sta.password));
    #endif
}


bool softApClass::wifiConnectDefault()
{
  bool ret = false;

  #ifdef DEBUG
  debugSerial.print(F("Connecting to SAVED AP: "));debugSerial.println(WiFi_SSID(true));
  debugSerial.print(F("Using Password: "));debugSerial.println(WiFi_psk(true));
  #endif

  ret = WiFi_enableSTA(true, storeSTAmode);
  delay(500); //why this delay
  #ifdef DEBUG
  debugSerial.print(F("Mode after delay: "));debugSerial.println(getModeString(WiFi.getMode()));
  if(!ret) debugSerial.println(F("[ERROR] wifi enableSta failed"));
  #endif

  ret = WiFi.begin();

  #ifdef DEBUG
  if(!ret) debugSerial.println(F("[ERROR] wifi begin failed"));
  #endif
  return ret;
  
}


void softApClass::resetSettings()
{

  #ifdef DEBUG
  debugSerial.println(F("resetSettings"));
  #endif

  WiFi_enableSTA(true,true); // must be sta to disconnect erase

  #ifdef ESP32
    WiFi.disconnect(true,true);
  #else
    WiFi.persistent(true);
    WiFi.disconnect(true);
    WiFi.persistent(false);
  #endif
  #ifdef DEBUG
  debugSerial.println((F("SETTINGS ERASED")));
  #endif

}










//utility functions
uint8_t softApClass::WiFi_softap_num_stations()
{
  #ifdef ESP8266
    return wifi_softap_get_station_num();
  #elif defined(ESP32_V010)
    return WiFi.softAPgetStationNum();
  #endif
}


void softApClass::setConfigPortalTimeout(unsigned long seconds)
{
  _configPortalTimeout = seconds * 1000;
}

String softApClass::toStringIp(IPAddress ip)
{
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}


int softApClass::getRSSIasQuality(int RSSI)
{
  int quality = 0;

  if (RSSI <= -100) 
  {
    quality = 0;
  } else if (RSSI >= -50) 
  {
    quality = 100;
  } else 
  {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}




void setMqttFlag(int mqtt_flag_addr, bool value)
{
  mqttflag = value;
  eepromUpdate(mqtt_flag_addr, (uint8_t*)&mqttflag, 1);
  debugSerial.print("Flag is: ");
  debugSerial.println(mqttflag);
  
}

void setBSFlag(int bs_flag_addr, bool value)
{
  bsflag = value;
  eepromUpdate(bs_flag_addr, (uint8_t*)&bsflag, 1);
  debugSerial.print("Flag is: ");
  debugSerial.println(bsflag);
}

bool retBSFlag(int bs_flag_addr)
{
    bool f;
    eepromRead(bs_flag_addr, (uint8_t*)&f, 1);
    return f;
}

bool retMqttFlag(int mqtt_flag_addr)
{
    bool f;
    eepromRead(mqtt_flag_addr, (uint8_t*)&f, 1);
    return f;
}




void eepromRead(uint32_t addr, uint8_t *buf, uint16_t len)
{
  EEPROM.begin(512);
  uint16_t eepAddr = (uint16_t)addr;
  uint8_t *ptr = buf;
  for (uint16_t i = 0 ; i < len; i++)
  {
    
    *(ptr + i) = EEPROM.read(eepAddr + i);
  }
}


void eepromUpdate(uint32_t addr, uint8_t *buf, uint16_t len)
{
  EEPROM.begin(512);
  uint16_t eepAddr = (uint16_t)addr;
  uint8_t *ptr = buf;
  for (uint16_t i = 0; i < len; i++)
  {
    EEPROM.write(eepAddr + i, *(ptr + i));
  }
  EEPROM.commit();
}



uint8_t checksumAP(void *buffer, uint8_t len)
{
  uint8_t *p = (uint8_t*)buffer;
  
  uint16_t sum = 0;
  uint8_t i;
  for(i = 0; i< len; i++)
  {
    sum += (uint16_t)p[i];
  }
  return (uint8_t)sum;
}


String softApClass::getModeString(uint8_t mode)
{
  if(mode <= 3) return WIFI_MODES[mode];
  return F("Unknown");
}


String softApClass::getWLStatusString(uint8_t status)
{
  if(status <= 7) return WIFI_STA_STATUS[status];
  return F("Unknown");
}

bool softApClass::WiFi_Mode(WiFiMode_t m, bool persistent)
{
  bool ret;
  #ifdef ESP8266
  if((wifi_get_opmode() == (uint8)m) && !persistent)
  {
    return true;
  }
  ETS_UART_INTR_DISABLE();
  if(persistent) ret = wifi_set_opmode(m);
  else ret = wifi_set_opmode_current(m);
  ETS_UART_INTR_ENABLE();
  return ret;

  #elif defined(ESP32_V010)
  if(persistent && esp32persistent) WiFi.persistent(true);
  ret = WiFi.mode(m);
  if(persistent && esp32persistent) WiFi.persistent(false);
  return ret;
  #endif        //Will start from here

}

bool softApClass::WiFi_Mode(WiFiMode_t m)
{
  return WiFi_Mode(m, false);
}




//Web portal handlers

void softApClass::handleRoot(void)
{

  #ifdef DEBUG
  debugSerial.println("handleRoot");
  #endif

  if(captivePortal()) return;

  handleRequest();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Welcome to Pranisheba");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(STYLE_FORM_POP_UP);
  
  page += FPSTR(HTTP_LOGIN_PAGE);
  page += String(F("</br>"));  
  page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  
  page += _apName;
  page += String(F("</h3>"));
  page += FPSTR(HTTP_HEADER_END);

  // if (true)
  // {
  //   int n = WiFi.scanNetworks();
  //   #ifdef DEBUG
  //   debugSerial.println("Scan Done");
  //   #endif

  //   if(n == 0)
  //   {
  //     #ifdef DEBUG
  //     debugSerial.println("No networks found");
  //     page += F("No networks found. Refresh to scan again.");
  //     #endif
  //   }
  //   else
  //   {
  //     int indices[n];
  //     for (int i = 0; i < n; i++)
  //     {
  //       indices[i] = i;
  //     }

  //     for (int i = 0; i < n; i++)
  //     {
  //       for (int j = i+1; j < n; j++)
  //       {
  //         if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
  //         {
  //           std::swap(indices[i], indices[j]);
  //         }
  //       }
  //     }

  //     String cssid;
  //     for (int i = 0; i < n; i++) 
  //     {
  //       if (indices[i] == -1) continue;
  //       cssid = WiFi.SSID(indices[i]);
  //       for (int j = i + 1; j < n; j++) 
  //       {
  //         if (cssid == WiFi.SSID(indices[j])) 
  //         {
  //           #ifdef DEBUG
  //           debugSerial.println(("DUP AP: " + WiFi.SSID(indices[j])));
  //           #endif
  //           indices[j] = -1; // set dup aps to index -1
  //         }
  //       }
  //     }

  //     for (int i = 0; i < n; i++) {
  //       if (indices[i] == -1) continue; // skip dups
  //       #ifdef DEBUG
  //       debugSerial.println(WiFi.SSID(indices[i]));
  //       debugSerial.println(WiFi.RSSI(indices[i]));
  //       #endif
  //       int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

  //       if (_minimumQuality == -1 || _minimumQuality < quality) {
  //         String item = FPSTR(HTTP_ITEM);
  //         String rssiQ;
  //         rssiQ += quality;
  //         item.replace("{v}", WiFi.SSID(indices[i]));
  //         item.replace("{r}", rssiQ);
  //         if (WiFi.encryptionType(indices[i]) != WM_WIFIOPEN) {
  //           item.replace("{i}", "l");
  //         } else {
  //           item.replace("{i}", "");
  //         }
  //         //debugSerial.println(item);
  //         page += item;
  //         delay(0);
  //       } else {
  //         #ifdef DEBUG
  //         debugSerial.println(F("Skipping due to quality"));
  //         #endif
  //       }

  //     }
  //     page += "<br/>";

  //   }
  // }

  page += FPSTR(HTTP_FORM_START);
  
  
  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_SCAN_LINK);

  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, F("text/html"), page);
  debugSerial.println(F("Root page sent"));

}




void softApClass::handleDevLogin(void)
{

  handleRequest();
  #ifdef DEBUG
  debugSerial.println("HTTP Param");
  #endif
  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Log in to Adorsho Pranisheba");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">"));
  page += _apName;
  page += String(F("</h3>"));
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_LOGIN_FORM_START);
  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, F("text/html"), page);


}

void softApClass::handleDevLoginSubmit(void)
{
  handleRequest();
  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Pranisheba Configure Server");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);

  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\" Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">"));
  page += _apName;
  page += String(F("</h3>"));
  page += FPSTR(HTTP_HEADER_END);
  
  #ifdef DEBUG
  debugSerial.println(F("Developer Login..."));
  #endif
  String _tempUserName = "", _tempPass = "";

  _tempUserName = server->arg("u").c_str();
  _tempPass     = server->arg("h").c_str();

  #ifdef DEBUG
  debugSerial.print(F("Received Username: "));debugSerial.println(_tempUserName);
  debugSerial.print(F("Received Password: "));debugSerial.println(_tempPass);
  #endif

  if(_tempUserName.equals(_httpUserName))
  {
    #ifdef DEBUG
    debugSerial.println("Username Matched");
    #endif
    if(_tempPass.equals(_httpPassWord))
    {
      #ifdef DEBUG
      debugSerial.println("Password Matched");
      #endif
      page += FPSTR(HTTP_PORTAL_OPTIONS);
    }
    else
    {
      #ifdef DEBUG
      debugSerial.println("Password Mismatched");
      #endif
      page += String(F("<h3 align=\"center\">Username and Password mismatched. Please try again.</h3>"));
      page += FPSTR(HTTP_LOGIN_PAGE);
    }
  }
  else
  {
    #ifdef DEBUG
    debugSerial.println(F("Username mismatched"));
    #endif
    page += FPSTR(HTTP_LOGIN_PAGE);
  }

  page += FPSTR(HTTP_END);
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text-html", page);

}


void softApClass::handleWifi(boolean scan)
{
  
  handleRequest();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Available Networks");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  if(scan)
  {
  	page += FPSTR(STYLE_FORM_POP_UP);
  }
  
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">")); 
  //page += _apName;
  page += String(F("</h1>"));
  page += String(F("<h3 align=\"center\">Adorsho Pranisheba</h3>"));
  page += FPSTR(HTTP_HEADER_END);

  if (scan) {
    int n = WiFi.scanNetworks();
    #ifdef DEBUG
    debugSerial.println(F("Scan done"));
    #endif
    if (n == 0) {
      #ifdef DEBUG
      debugSerial.println(F("No networks found"));
      #endif
      page += F("No networks found. Refresh to scan again.");
    } else {

      //sort networks
      int indices[n];
      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }

      // RSSI SORT

      // old sort
      for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
          if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
            std::swap(indices[i], indices[j]);
          }
        }
      }

      /*std::sort(indices, indices + n, [](const int & a, const int & b) -> bool
        {
        return WiFi.RSSI(a) > WiFi.RSSI(b);
        });*/

      // remove duplicates ( must be RSSI sorted )
      
        String cssid;
        for (int i = 0; i < n; i++) 
        {
          if (indices[i] == -1) continue;
          cssid = WiFi.SSID(indices[i]);
          for (int j = i + 1; j < n; j++) 
          {
            if (cssid == WiFi.SSID(indices[j])) 
            {
              #ifdef DEBUG
              debugSerial.println("DUP AP: " + WiFi.SSID(indices[j]));
              #endif
              indices[j] = -1; // set dup aps to index -1
            }
          }
        }
      

      //display networks in page
      for (int i = 0; i < n; i++) {
        if (indices[i] == -1) continue; // skip dups

        #ifdef DEBUG
        debugSerial.println(WiFi.SSID(indices[i]));
        debugSerial.println(WiFi.RSSI(indices[i]));
        #endif
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

        if (_minimumQuality == -1 || _minimumQuality < quality) {
          String item = FPSTR(HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
          if (WiFi.encryptionType(indices[i]) != WM_WIFIOPEN) {
            item.replace("{i}", "l");
          } else {
            item.replace("{i}", "");
          }
          //debugSerial.println(item);
          page += item;
          delay(0);
        } else {
          #ifdef DEBUG
          debugSerial.println(F("Skipping due to quality"));
          #endif
        }

      }
      page += "<br/>";
    }
  }

  page += FPSTR(HTTP_FORM_START);
  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_SCAN_LINK);

  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}









void softApClass::handleBSConfig(void)
{


  handleRequest();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Base Station configuration Form | Adorsho Pranisheba");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));
  page += FPSTR(HTTP_HEADER_END);
  page += String(F("</head><body><div style='margin-left:500px;text-align:left;display:inline-block;min-width:260px;'>"));

  
  page += FPSTR(HTTP_BS_FORM_START);
  
  page += FPSTR(HTTP_END);
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}

void softApClass::previewBSInfo(void)
{
  handleRequest();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Base Station Configuration");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));

  #ifdef DEBUG
  debugSerial.println(F("Saving Base Station Configuration..."));
  #endif
  String _tempId="",_tempMomentDuration="",_tempMaxNode="",_tempReserveNode="",_tempMaxTrayNode="";
  String _tempP00 = "",_tempP01 = "",_tempP02 = "",_tempP03 = "",_tempP04 = "";
  String _tempP10 = "",_tempP11 = "",_tempP12 = "",_tempP13 = "",_tempP14 = "";
  String _tempP2_5_0 = "",_tempP2_5_1 = "",_tempP2_5_2 = "",_tempP2_5_3 = "";

  _tempId 	          = server->arg("bsId").c_str();
  _tempMomentDuration      = server->arg("momentDuration").c_str();


  _tempP00 		      = server->arg("bsPipe00").c_str();
  _tempP01 		      = server->arg("bsPipe01").c_str();
  _tempP02 		      = server->arg("bsPipe02").c_str();
  _tempP03 		      = server->arg("bsPipe03").c_str();
  _tempP04 		      = server->arg("bsPipe04").c_str();


  _tempP10 		      = server->arg("bsPipe10").c_str();
  _tempP11 		      = server->arg("bsPipe11").c_str();
  _tempP12		      = server->arg("bsPipe12").c_str();
  _tempP13 		      = server->arg("bsPipe13").c_str();
  _tempP14 		      = server->arg("bsPipe14").c_str();


  _tempP2_5_0 		  = server->arg("bsPipe2_5_0").c_str();
  _tempP2_5_1 		  = server->arg("bsPipe2_5_1").c_str();
  _tempP2_5_2 	    = server->arg("bsPipe2_5_2").c_str();
  _tempP2_5_3       = server->arg("bsPipe2_5_3").c_str();
  
  
  _tempMaxNode 		  = server->arg("maxNode").c_str();
  _tempReserveNode	= server->arg("reserveNode").c_str();
  _tempMaxTrayNode 	= server->arg("maxTrayNode").c_str();
  
  #ifdef DEBUG
  debugSerial.println(F("Received ID : "));
  debugSerial.println(_tempId);
  debugSerial.println(F("Received Moments : "));
  debugSerial.println(_tempMomentDuration);
  debugSerial.println(F("Received Base Station Pipe0 Address : "));
  debugSerial.println(_tempP00);debugSerial.println(_tempP01);debugSerial.println(_tempP02);debugSerial.println(_tempP03);debugSerial.println(_tempP04);
  debugSerial.println(F("Received Base Station Pipe1 Address : "));
  debugSerial.println(_tempP10);debugSerial.println(_tempP11);debugSerial.println(_tempP12);debugSerial.println(_tempP13);debugSerial.println(_tempP14);
  debugSerial.println(F("Received Base Station Pipe2 to Pipe5 Address : "));
  debugSerial.println(_tempP2_5_0);debugSerial.println(_tempP2_5_1);debugSerial.println(_tempP2_5_2);debugSerial.println(_tempP2_5_3);
  debugSerial.println(F("Received Max Node : "));
  debugSerial.println(_tempMaxNode);
  debugSerial.println(F("Received Reserve Node : "));
  debugSerial.println(_tempReserveNode);
  debugSerial.println(F("Received Max Tray Node : "));
  debugSerial.println(_tempMaxTrayNode);
  #endif
  

  bsConf.bsId             = _tempId.toInt();
  bsConf.momentDuration        = _tempMomentDuration.toInt();

  bsConf.bsPipe0Addr[0]   = _tempP00.toInt();
  bsConf.bsPipe0Addr[1]   = _tempP01.toInt();
  bsConf.bsPipe0Addr[2]   = _tempP02.toInt();
  bsConf.bsPipe0Addr[3]   = _tempP03.toInt();
  bsConf.bsPipe0Addr[4]   = _tempP04.toInt();

  bsConf.bsPipe1Addr[0]   = _tempP10.toInt();
  bsConf.bsPipe1Addr[1]   = _tempP11.toInt();
  bsConf.bsPipe1Addr[2]   = _tempP12.toInt();
  bsConf.bsPipe1Addr[3]   = _tempP13.toInt();
  bsConf.bsPipe1Addr[4]   = _tempP14.toInt();

  bsConf.pipe2to5Addr[0]   = _tempP2_5_0.toInt();
  bsConf.pipe2to5Addr[1]   = _tempP2_5_1.toInt();
  bsConf.pipe2to5Addr[2]   = _tempP2_5_2.toInt();
  bsConf.pipe2to5Addr[3]   = _tempP2_5_3.toInt();

  bsConf.maxNode      = _tempMaxNode.toInt();
  bsConf.reserveNode  = _tempReserveNode.toInt();
  bsConf.maxTrayNode  = _tempMaxTrayNode.toInt();
  bsConf.checksum = 0;
  bsConf.checksum = checksumAP((void* )&bsConf, sizeof(bsConfig_t));

  
  bsflag = true;
  

  page += F("<dl>");
  page += F("<dt>Base Station ID : </dt><dd>");
  page += bsConf.bsId;
  page += F("</dd>");
  page += F("<dt>Moments : </dt><dd>");
  page += bsConf.momentDuration;
  page += F("</dd>");
  page += F("<dt>Base Station Pipe0 Address : </dt><dd>");
  page += bsConf.bsPipe0Addr[0];page += F(" ");page += bsConf.bsPipe0Addr[1];page += F(" ");page += bsConf.bsPipe0Addr[2];page += F(" ");page += bsConf.bsPipe0Addr[3];page += F(" ");page += bsConf.bsPipe0Addr[4];
  page += F("</dd>");
  page += F("<dt>Base Station Pipe1 Address : </dt><dd>");
  page += bsConf.bsPipe1Addr[0];page += F(" ");page += bsConf.bsPipe1Addr[1];page += F(" ");page += bsConf.bsPipe1Addr[2];page += F(" ");page += bsConf.bsPipe1Addr[3];page += F(" ");page += bsConf.bsPipe1Addr[4];
  page += F("</dd>");
  page += F("<dt>Base Station Pipe2 to Pipe5 Address: </dt><dd>");
  page += bsConf.pipe2to5Addr[0];page += F(" ");page += bsConf.pipe2to5Addr[1];page += F(" ");page += bsConf.pipe2to5Addr[2];page += F(" ");page += bsConf.pipe2to5Addr[3];
  page += F("</dd>");
  page += F("<dt>Max Node : </dt><dd>");
  page += bsConf.maxNode;
  page += F("</dd>");
  page += F("<dt>Reverse Node : </dt><dd>");
  page += bsConf.reserveNode;
  page += F("</dd>");
  page += F("<dt>MaxTray Node: </dt><dd>");
  page += bsConf.maxTrayNode;
  page += F("</dd>");
  
  

  page += FPSTR(HTTP_BS_FORM_EDIT);
  page += FPSTR(HTTP_BS_FORM_SAVE);
  

  
  page += FPSTR(HTTP_END);    
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}

void softApClass::saveBSInfo(void)
{
  handleRequest();
  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Base Station configuration Saved | Adorsho Pranisheba");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));

  #ifdef DEBUG
  debugSerial.println(F("Saving Base Station Configuration..."));
  #endif


  bsflag = true;
  setBSFlag(BS_FLAG_ADDRESS, bsflag);
  #ifdef DEBUG
  debugSerial.println(F("BS config done"));
  #endif

  page += String(F("<h3 align=\"center\">Base Station Credentials saved successfully.</h3>"));

  page += F("<br/><form action=\"/home\" method=\"post\"><button>Home</button></form>");



  page += FPSTR(HTTP_END);    
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}






















void softApClass::handleMqttConfig(void)
{

  
  
  handleRequest();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "MQTT configuration Form | Adorsho Pranisheba");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_MQTT_FORM_START);
  
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}

void softApClass::previewMqttInfo(void)
{
  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "MQTT configuration");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));

  #ifdef DEBUG
  debugSerial.println(F("Saving MQTT Configuration..."));
  #endif
  String _tempServer="",_tempPort="", _tempCid="",_tempUserName = "",_tempPass="",_tempT1="",_tempT2="",_tempT3="",_tempT4="";

  _tempServer 	= server->arg("ip").c_str();
  _tempPort     = server->arg("port").c_str();
  _tempCid 		= server->arg("cid").c_str();
  _tempUserName = server->arg("musername").c_str();
  _tempPass 	= server->arg("mqpass").c_str();
  _tempT1 		= _tempUserName + "/" + server->arg("mptopic").c_str();
  _tempT2 		= _tempUserName + "/" + server->arg("mltopic").c_str();
  _tempT3 		= _tempUserName + "/" + server->arg("mcatopic").c_str();
  _tempT4 		= _tempUserName + "/" + server->arg("mlwtopic").c_str();


  #ifdef DEBUG
  debugSerial.println(F("Received IP : "));
  debugSerial.println(_tempServer);
  debugSerial.println(F("Received Port: "));
  debugSerial.println(_tempPort);
  debugSerial.println(F("Received Client ID: "));
  debugSerial.println(_tempCid);
  debugSerial.println(F("Received Username : "));
  debugSerial.println(_tempUserName);
  debugSerial.println(F("Received Password: "));
  debugSerial.println(_tempPass);
  debugSerial.println(F("Received Payload Topic : "));
  debugSerial.println(_tempT1);
  debugSerial.println(F("Received Log Topic: "));
  debugSerial.println(_tempT2);
  debugSerial.println(F("Received Conf Ack Topic : "));
  debugSerial.println(_tempT3);
  debugSerial.println(F("Received Last Will Topic: "));
  debugSerial.println(_tempT4);
  #endif

  strcpy(serverMqtt.mqttServer,_tempServer.c_str());
  strcpy(serverMqtt.mqttClientId,_tempCid.c_str());
  strcpy(serverMqtt.mqttUserName,_tempUserName.c_str());
  strcpy(serverMqtt.mqttPassword,_tempPass.c_str());
  strcpy(serverMqtt.mqttPayloadtopic,_tempT1.c_str());
  strcpy(serverMqtt.mqttLogtopic,_tempT2.c_str());
  strcpy(serverMqtt.mqttConfigAcktopic,_tempT3.c_str());
  strcpy(serverMqtt.mqttLastWilltopic,_tempT4.c_str());
  serverMqtt.checksum = 0;
  serverMqtt.checksum = checksumAP((void* )&serverMqtt, sizeof(mqtt_credentials));

  serverMqtt.mqttPort = (uint16_t)_tempPort.toInt();
  serverMqtt.isConfigChanged = true;
  
  

  page += F("<dl>");
  page += F("<dt>MQTT SERVER: </dt><dd>");
  page += serverMqtt.mqttServer;
  page += F("</dd>");
  page += F("<dt>MQTT PORT: </dt><dd>");
  page += serverMqtt.mqttPort;
  page += F("</dd>");
  page += F("<dt>MQTT Client ID: </dt><dd>");
  page += serverMqtt.mqttClientId;
  page += F("</dd>");
  page += F("<dt>MQTT Username: </dt><dd>");
  page += serverMqtt.mqttUserName;
  page += F("</dd>");
  page += F("<dt>MQTT Password: </dt><dd>");
  page += serverMqtt.mqttPassword;
  page += F("</dd>");
  page += F("<dt>MQTT Payload Topic: </dt><dd>");
  page += serverMqtt.mqttPayloadtopic;
  page += F("</dd>");
  page += F("<dt>MQTT Log Topic: </dt><dd>");
  page += serverMqtt.mqttLogtopic;
  page += F("</dd>");
  page += F("<dt>MQTT Config Ack Topic: </dt><dd>");
  page += serverMqtt.mqttConfigAcktopic;
  page += F("</dd>");
  page += F("<dt>MQTT Last Will Topic: </dt><dd>");
  page += serverMqtt.mqttLastWilltopic;
  page += F("</dd>");

  page += FPSTR(HTTP_MQTT_FORM_EDIT);
  page += FPSTR(HTTP_MQTT_FORM_SAVE);
  

  
  page += FPSTR(HTTP_END);    
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}


void softApClass::saveMqttInfo(void)
{
  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "MQTT configuration Saved | Adorsho Pranisheba");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));

  #ifdef DEBUG
  debugSerial.println(F("Saving MQTT Configuration..."));
  #endif


  mqttflag = true;
  setMqttFlag(MQTT_FLAG_ADDRESS, mqttflag);

  page += String(F("<h3 align=\"center\">Mqtt Credentials saved successfully.</h3>"));

  page += F("<br/><form action=\"/home\" method=\"post\"><button>Home</button></form>");



  page += FPSTR(HTTP_END);    
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}



















void softApClass::handleInfo() 
{

  #ifdef DEBUG
  debugSerial.println(F("Info"));
  #endif
  handleRequest();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);



  #ifdef ESP8266
  page += F("<dl>");
  page += F("<dt>Chip ID</dt><dd>");
  page += WIFI_getChipId();
  page += F("</dd>");
  page += F("<dt>Flash Chip ID</dt><dd>");
  page += ESP.getFlashChipId();
  page += F("</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  page += F("<dt>Real Flash Size</dt><dd>");
  page += ESP.getFlashChipRealSize();
  page += F(" bytes</dd>");


  #elif defined(ESP32_V010)
  page += F("<dl>");
  page += F("<dt>Chip ID</dt><dd>");
  page += WIFI_getChipId();
  page += F("</dd>");
  page += F("<dt>Chip model</dt><dd>");
  page += ESP.getChipModel();
  page += F("</dd>");
  page += F("<dt>Chip cores: </dt><dd>");
  page += ESP.getChipCores();
  page += F("</dd>");
  
  #endif




  page += F("<dt>Soft AP IP</dt><dd>");
  page += WiFi.softAPIP().toString();
  page += F("</dd>");
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  page += F("</dl>");
  
  // page += FPSTR(HTTP_WINDOW_CLOSE);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  #ifdef DEBUG
  debugSerial.println(F("Sent info page"));
  #endif
}



void softApClass::handleReset()
{
  
  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Reset");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);

  page += F("ESP is goint to be restarted!");

  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  #ifdef DEBUG
  debugSerial.println("Rebooting ESP");
  #endif
  delay(5000);
  ESP.restart();

}


bool softApClass::shutdownConfigPortal()
{
  #ifdef DEBUG
  debugSerial.println(F("Shutdown Config Portal"));
  #endif

  if(webPortalActive) return false;

  if(configPortalActive)
  {
    dnsServer->processNextRequest();
  }
  server->handleClient();
  server->stop();
  server.reset();

  WiFi.scanDelete();

  if(!configPortalActive) return false;

  dnsServer->stop();
  dnsServer.reset();


  bool ret = false;
  ret = WiFi.softAPdisconnect(false);

  #ifdef DEBUG
  if(!ret) debugSerial.println(F("[ERROR] disconnect configportal - softAPdisconnect FAILED"));
  debugSerial.print("Restoring usermode: ");debugSerial.println(getModeString(_usermode));
  #endif
  delay(1000);

  WiFi_Mode(_usermode); 

  if(WiFi.status() == WL_IDLE_STATUS)
  {
    // WiFi.reconnect();
    String _espSsid = WiFi.SSID();
    String _espPass = WiFi.psk();
  #if defined(ESP32_V010)
    WiFi.begin(_espSsid.c_str(),_espPass.c_str());
  #else
    WiFi.begin(_espSsid,_espPass);
  #endif
    delay(2000);

    #ifdef DEBUG
    debugSerial.println("WiFi Reconnect, was idle");
    #endif
  }
  #ifdef DEBUG
  debugSerial.print("Wifi status: ");debugSerial.println(getWLStatusString(WiFi.status()));
  debugSerial.print("Wifi mode: ");debugSerial.println(getModeString(WiFi.getMode()));
  #endif

  configPortalActive = false;
  return ret;


}


void softApClass::handleWifiSave()
{
  #ifdef DEBUG
  debugSerial.println(F("WiFi Save"));
  #endif

  _ssid = server->arg("s").c_str();
  _pass = server->arg("p").c_str();

  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));
  page += FPSTR(HTTP_HEADER_END);
  page += FPSTR(HTTP_SAVED);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  #ifdef DEBUG
  debugSerial.println(F("Sent wifi save page"));
  #endif

  connect = true; //signal ready to connect/reset

}

void softApClass::httpHome(void)
{
  String page = FPSTR(HTTP_HEADER);
  page.replace("{v}", "Portal");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  //page += FPSTR(PRANISHEBA_LOGO);
  page += String(F("<h1 align=\"center\">Adorsho Pranisheba</h1>"));
  page += String(F("<h3 align=\"center\">")); 
  page += _apName;
  page += String(F("</h3>"));

  page += FPSTR(HTTP_PORTAL_OPTIONS);

  page += FPSTR(HTTP_END);    
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}
