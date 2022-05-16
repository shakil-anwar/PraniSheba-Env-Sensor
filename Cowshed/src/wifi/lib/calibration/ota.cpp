#include "ota.h"
// #include "iotClient.h"
// #include "param.h"


#if defined(ESP32_V010)

bool otaFlag = false;

HTTPClient http;
#define FIRMWIRE_VERSION "3.0" // this is defined in param
String FirmwareVersion = { FIRMWIRE_VERSION }; //FIRMWIRE_VERSION

#define URL_fw_Version "https://raw.githubusercontent.com/mahmudulislam299/ota_test/master/bin-version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/mahmudulislam299/ota_test/master/Cowshed.ino.bin"
// #define URL_fw_Version "http://127.0.0.1:5500/bin-version.txt"
// #define URL_fw_Bin "http://127.0.0.1:5500/BS_Master.ino.bin"


const char * rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----\n";



int FirmwareVersionCheck(void) 
{
  String payload;
  int httpCode;
   Serial.println("Check for Firmware Update...");
   Serial.println(URL_fw_Version);
    if (http.begin(URL_fw_Version,rootCACertificate)) 
    {
         Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        delay(100);
        httpCode = http.GET();
        delay(100);
        if (httpCode == HTTP_CODE_OK) // if version received
        {
            payload = http.getString(); // save received version
        } 
        else 
        {
             Serial.print("Error Occured During Version Check: ");
             Serial.println(httpCode);
        }
        http.end();
    }

    if (httpCode == HTTP_CODE_OK) // if version received
    {
        payload.trim();
        if (payload.equals(FirmwareVersion)) 
        {
             Serial.printf("\nDevice  IS Already on Latest Firmware Version:%s\n", FirmwareVersion);
            return 0;
        } 
        else 
        {
             Serial.println(payload);
             Serial.println("New Firmware Detected");
            return 1;
        }
    }
    return 0;
}


void firmwareUpdate(void) 
{
     Serial.println("<---------Firmware updating start------------>");
    http.begin(URL_fw_Bin,rootCACertificate);

    t_httpUpdate_return ret = httpUpdate.update(http);

     Serial.println("<---------Firmware update finished------------>");

    switch (ret) 
    {
        case HTTP_UPDATE_FAILED:
             Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
             Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
             Serial.println("HTTP_UPDATE_OK");
            break;
    }
    http.end();
}




void otaBegin()
{
     Serial.print("<------------------------Active Firmware Version:");
     Serial.print(FirmwareVersion);
     Serial.println(" ------------------------>");
}

void otaLoop()
{
  
  // if ( Serial.available() > 0) 
  // {
  //   int incomingByte =  Serial.read();
  //   if (incomingByte == 'U') 
  //   {
  //     if(netState.internetAvail) // if (pingStatus)
  //     {
  //       otaFlag = true;
  //     }
      
  //   }
  // }


  if(otaFlag)
  {  
    otaFlag = false;
    
    if (FirmwareVersionCheck()) 
    {
      firmwareUpdate();
    }
  }


}


// void otaBegin()
// {
//    ArduinoOTA.setPort(3232);  // Port defaults to 3232
//    ArduinoOTA.setHostname("praniShebaOTA");  // Hostname defaults to esp3232-[MAC]
// //   ArduinoOTA.setPassword("admin");  // No authentication by default
// //   MD5(admin) = 21232f297a57a5a743894a0e4a801fc3  // Password can be set with it's md5 value as well
// //   ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

//   ArduinoOTA
//     .onStart([]() {
//       String type;
//       if (ArduinoOTA.getCommand() == U_FLASH)
//         type = "sketch";
//       else // U_SPIFFS
//         type = "filesystem";

//       // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
//        Serial.println("Start updating " + type);
//     })
//     .onEnd([]() {
//        Serial.println("\nEnd");
//     })
//     .onProgress([](unsigned int progress, unsigned int total) {
//        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//     })
//     .onError([](ota_error_t error) {
//        Serial.printf("Error[%u]: ", error);
//       if (error == OTA_AUTH_ERROR)  Serial.println("Auth Failed");
//       else if (error == OTA_BEGIN_ERROR)  Serial.println("Begin Failed");
//       else if (error == OTA_CONNECT_ERROR)  Serial.println("Connect Failed");
//       else if (error == OTA_RECEIVE_ERROR)  Serial.println("Receive Failed");
//       else if (error == OTA_END_ERROR)  Serial.println("End Failed");
//     });

//   ArduinoOTA.begin();
//    Serial.println("OTA begin done");
// }


// void otaLoop()
// {
//    ArduinoOTA.handle();
// }

#endif