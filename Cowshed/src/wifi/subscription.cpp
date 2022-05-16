#include "subscription.h"

#if defined(SUBCRIPTION_ON)

WiFiClient      iotWifiClient;
HTTPClient      httpIot;
// String authUsername = "himel@gmail.com";
// String authPassword = "pranisheba1234";
String authUsername = "devices@pranisheba.com.bd";
String authPassword = "av56H0Ku6JuR";

String getTokenApi = "http://iotdev.pranisheba.com.bd/api/v1/api-token-auth/";
String deviceSubscriptionApi = "http://iotdev.pranisheba.com.bd/api/v1/devices/is_expired/?c_id=G3006";
// String gasSensorID = "http://iotdev.pranisheba.com.bd/api/v1/devices/is_expired/?c_id=G3006";
//+ String(RCMS_DEVICE_ID);
String tokenParam = "token";
String isExpired = "is_expired";
String dayRemianing = "remaining_days";



JSONVar getJsonValue(String jsonKey, String jsonPayload);



bool getSubscriptionStatusWifi()
{
  bool status = false;
  
  static uint32_t lastChkTime = 0;
  int httpResponseCode;

  if((millis()-lastChkTime)>60000 && subscribeParam.isDeviceHasSubscription)
  {
    if(netState.internetAvail)
    {
        String tokenVal = getAuthToken();
        if (tokenVal != NULL) 
        {
            // Get Subscription status for specific device 
            httpIot.begin(iotWifiClient, deviceSubscriptionApi);
            httpIot.addHeader("Authorization", "token " + tokenVal);
        
            httpResponseCode = httpIot.GET(); 
        
            if (httpResponseCode > 0)
            {   //Check for the returning code
                String payload = httpIot.getString();
                Serial.print("getSubscriptionStatus: ");
                Serial.println(payload);
                // Serial.print("JSON Value: ");
                // Serial.println((const char*)getJsonValue(isExpired,payload));
                status = (bool)getJsonValue(isExpired,payload);
                subscribeParam.isSubscriptionOn = status;
                subscribeParam.remainingDays = (int)getJsonValue(dayRemianing,payload);             
                Serial.print("Subscription Status: ");
                Serial.println(subscribeParam.isSubscriptionOn);
                Serial.print("Remaining days: ");
                Serial.println(subscribeParam.remainingDays);
            }
            httpIot.end();
        }
    }
    lastChkTime = millis();
  }
  return status;  
}

String getAuthTokenWifi()
{
    httpIot.begin(iotWifiClient, getTokenApi);
    // Specify content-type header
    httpIot.addHeader("Content-Type", "application/json");
    String auth = "{\"username\": \"" + authUsername + "\", \"password\": \"" + authPassword+"\"}";
    int httpResponseCode = httpIot.POST(auth);
    String payload = ""; 

    if (httpResponseCode>0) 
    {
        Serial.print("HTTP Auth Response code: ");
        Serial.println(httpResponseCode);
        payload = httpIot.getString();
        httpIot.end();
        // Serial.print("Payload: ");
        // Serial.println(payload);
        if(payload.length() > 0)
        {
            String tokenValue((const char*) getJsonValue(tokenParam, payload));
            return tokenValue;
        }
    }
    else 
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    return "";
}

JSONVar getJsonValue(String jsonKey, String jsonPayload)
{
    JSONVar tokenJson = JSON.parse(jsonPayload);
    if (JSON.typeof(tokenJson) != "undefined") 
    {
        if(tokenJson.hasOwnProperty(jsonKey))
        {
            // String tokenValue((const char*)tokenJson[jsonKey]);
            // Serial.print("JSON Value: ");
            // Serial.println((int)tokenJson[jsonKey], BIN);
            return tokenJson[jsonKey];
        }
        else
        {
            Serial.print("Json don't have property ");
            return NULL;
        }
    }
    else
    {
        Serial.println("Parsing input failed!");
        return NULL;
    }
}

#endif