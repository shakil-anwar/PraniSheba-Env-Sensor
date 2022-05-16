#ifndef _SUBSCRIPTION_H_
#define _SUBSCRIPTION_H_

#include "wifi.h"
#include "sim800.h"
#include "../common_lib/Arduino_JSON/src/Arduino_JSON.h"

#if defined(SUBCRIPTION_ON)
#include <HTTPClient.h>

String getAuthTokenWifi();
bool getSubscriptionStatusWifi();


#endif


#endif      //_SUBSCRIPTION_H_