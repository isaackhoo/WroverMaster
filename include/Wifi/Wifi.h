#pragma once

#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include "Helper/Helper.h"

using namespace Helper;

namespace Wifi
{
    extern const String ssid;
    extern const String password;

    extern bool ConnectWifi();
    extern bool isWifiConnected();

    extern String getCurrentDate();
    extern String getCurrentTime();
}; // namespace Wifi

#endif