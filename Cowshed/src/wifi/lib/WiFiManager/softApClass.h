#ifndef SOFT_AP_CLASS_H
#define SOFT_AP_CLASS_H

#include <cstdint>
#include "EEPROM.h"
#include "Arduino.h"
#if defined(PROD_BUILD)
#include "../../../common_lib/iotStructure/iotStructure.h"
#else
#include "iotStructure.h"
#endif 

#if defined(ESP8266) || defined(ESP32_V010)

#ifdef ESP8266
#include <core_version.h>
#endif

#include <vector>

// #define WM_MDNS            // includes MDNS, also set MDNS with sethostname
// #define WM_FIXERASECONFIG  // use erase flash fix
// #define WM_ERASE_NVS       // esp32 erase(true) will erase NVS 
// #define WM_RTC             // esp32 info page will include reset reasons

// #define WM_JSTEST                      // build flag for enabling js xhr tests
// #define WIFI_MANAGER_OVERRIDE_STRINGS // build flag for using own strings include

#ifdef ARDUINO_ESP8266_RELEASE_2_3_0
#warning "ARDUINO_ESP8266_RELEASE_2_3_0, some WM features disabled" 
#define WM_NOASYNC         // esp8266 no async scan wifi
#define WM_NOCOUNTRY       // esp8266 no country
#define WM_NOAUTH          // no httpauth
#define WM_NOSOFTAPSSID    // no softapssid() @todo shim
#endif

// #include "soc/efuse_reg.h" // include to add efuse chip rev to info, getChipRevision() is almost always the same though, so not sure why it matters.

// #define esp32autoreconnect    // implement esp32 autoreconnect event listener kludge, @DEPRECATED
// autoreconnect is WORKING https://github.com/espressif/arduino-esp32/issues/653#issuecomment-405604766

#define WM_WEBSERVERSHIM      // use webserver shim lib
#define DEBUG

#ifdef ESP8266

    extern "C" {
      #include "user_interface.h"
    }
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>

    #ifdef WM_MDNS
        #include <ESP8266mDNS.h>
    #endif
    

    #define WIFI_getChipId() ESP.getChipId() 
    #define WM_WIFIOPEN   ENC_TYPE_NONE

#elif defined(ESP32_V010)

    // debugSerial.println("Hello ESP32");
    #include <WiFi.h>
    #include <esp_wifi.h>  
    #include <Update.h>
    
    #define WIFI_getChipId() (uint32_t)ESP.getEfuseMac() 
    #define WM_WIFIOPEN   WIFI_AUTH_OPEN

    #ifndef WEBSERVER_H
        #ifdef WM_WEBSERVERSHIM
            #include <WebServer.h>
        #else
            #include <ESP8266WebServer.h>
        #endif
    #endif

    #ifdef WM_ERASE_NVS
       #include <nvs.h>
       #include <nvs_flash.h>
    #endif

    #ifdef WM_MDNS
        #include <ESPmDNS.h>
    #endif

    #ifdef WM_RTC
        #include <rom/rtc.h>
    #endif

#else
#endif

#include <DNSServer.h>
#include <memory>
#include "strings_en.h"

// #define debugSerial Serial1                          //Define debug port

#if defined(BOARD_MEGA1284_V010) && !defined(ESP32_V010)
#define debugSerial Serial1
#else
#define debugSerial Serial
#endif





class softApClass
{
    public:


    bool startConfigPortal(char const *apName, char const *apPass);
    bool startConfigPortal(char const *apName);
    uint8_t handleConfigPortal();
    void setConfigPortalTimeout(unsigned long seconds);
    bool wifiConnectDefault();
    void resetSettings();
 
    





    std::unique_ptr<DNSServer>        dnsServer;

    #if defined(ESP32_V010) && defined(WM_WEBSERVERSHIM)
        using WM_WebServer = WebServer;
    #else
        using WM_WebServer = ESP8266WebServer;
    #endif

    std::unique_ptr<WM_WebServer>     server;




    private:

    const byte    DNS_PORT                = 53;
    const byte    HTTP_PORT               = 80;



    int _minimumQuality = -1;  
    uint16_t _httpPort = 80;
    int timer = 0; //To check time interval in order to print number of clients
    
    uint8_t       _connectRetries = 3;
    unsigned long _configPortalTimeout = 0;
    unsigned long _connectTimeout = 0;
    unsigned long _configPortalStart = 0;
    unsigned long _saveTimeout = 0; 
    int _cpclosedelay = 2000;

    bool _enableCaptivePortal = true;
    bool _configPortalIsBlocking = true;
    bool _apClientCheck = false;
    bool _webClientCheck = true;
    bool _debug = true;
    bool esp32persistent = false;
    bool webPortalActive = false;
    bool storeSTAmode = true; 
    bool _connectonsave = true;

    bool connect;
    bool configPortalActive = false;

    String        _apName = "";
    String        _apPass = "";
    String        _httpUserName = "admin";
    String        _httpPassWord = "12345";
    String        _ssid = "";
    String        _pass = "";

    WiFiMode_t    _usermode = WIFI_STA;



    void setupConfigPortal();
    uint8_t processConfigPortal();
    bool shutdownConfigPortal();
    bool captivePortal();
    bool configPortalHasTimeout();
    void setupDNSD();
    void handleRequest();

    uint8_t WiFi_softap_num_stations();
    String toStringIp(IPAddress ip);
    String getModeString(uint8_t mode);
    String getWLStatusString(uint8_t status);
    bool WiFi_Mode(WiFiMode_t m, bool persistent);
    bool WiFi_Mode(WiFiMode_t m);
    uint8_t connectWifi(String ssid, String pass, bool connect);
    bool wifiConnectNew(String ssid, String pass, bool connect);
    bool WiFi_enableSTA(bool enable, bool persistent);
    bool WiFi_enableSTA(bool enable);
    bool WiFi_Disconnect();
    uint8_t waitForConnectResult(uint32_t timeout);
    uint8_t waitForConnectResult();
    String WiFi_SSID(bool persistent) const;
    String WiFi_psk(bool persistent) const;
    bool WiFi_hasAutoConnect();
    


    int getRSSIasQuality(int RSSI);
    


    //Web page handler functions
    void        handleRoot(void);
    void        handleDevLogin(void);
    void        handleDevLoginSubmit(void);
    void        handleWifi(boolean scan);
    void        handleWifiSave();
    void        handleBSConfig(void);
    void        previewBSInfo(void);
    void        saveBSInfo(void);
    void        handleMqttConfig(void);
    void        previewMqttInfo(void);
    void        saveMqttInfo(void);
    void        handleInfo();
    void        handleReset();
    void        httpHome(void);
    





    std::function<void(softApClass*)> _apcallback;
    std::function<void()> _webservercallback;
    std::function<void()> _savewificallback;
    std::function<void()> _presavecallback;
    std::function<void()> _saveparamscallback;
    std::function<void()> _resetcallback;




};


void            setMqttFlag(int mqtt_flag_addr, bool value);
bool            retMqttFlag(int mqtt_flag_addr);
void            setBSFlag(int bs_flag_addr, bool value);
bool            retBSFlag(int bs_flag_addr);
void            eepromUpdate(uint32_t addr, uint8_t *buf, uint16_t len);
void            eepromRead(uint32_t addr, uint8_t *buf, uint16_t len);
uint8_t         checksumAP(void *buffer, uint8_t len);



#endif



extern mqtt_credentials serverMqtt;
extern bsConfig_t bsConf;


#endif




