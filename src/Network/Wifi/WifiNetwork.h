#pragma once

#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include "../include/common.h"
#include <time.h>
#include <WiFi.h>
#include <WString.h>
#include "../../Helpers/Local/LocalHelper.h"

// -----------------------------
// NETWORK ssids and pass
// -----------------------------
// const String ssid = "SINGTEL-7591";
// const String password = "khoo0011212827";

// -----------------------------
// NETWORK VARIABLE DECLARATION
// -----------------------------
extern const String ssid;
extern const String password;

// wifi connection variables
extern const unsigned long connectionTimeout;
extern unsigned long startTime;

// date time settings
extern long timezone;
extern byte daysavetime;
extern int dateTimeServerResponseTimeout;
extern const int baseYear;
extern const int mthIncrement;
extern const String timeDelimiter;

// -----------------------------
// NETWORK METHODS DECLARATION
// -----------------------------
extern bool ConnectWifi();
extern bool isWifiConnected();

tm getDateTime();
extern String getCurrentDate();
extern String getCurrentTime();

#endif
