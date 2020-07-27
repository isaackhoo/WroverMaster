#include "./WifiNetwork.h"

const unsigned long connectionTimeout = 1000 * 5; // 5s
unsigned long startTime = 0;

long timezone = 8;                       // GMT+8
byte daysavetime = 0;                    // no daylight savings
int dateTimeServerResponseTimeout = 200; // 200ms
const int baseYear = 1900;
const int mthIncrement = 1;
const String timeDelimiter = ":";

bool ConnectWifi()
{
    // connect to network
    WiFi.begin(ssid.c_str(), password.c_str());
    startTime = millis();

    // check if connection has been established
    while (WiFi.status() != WL_CONNECTED)
    {
        // if connection not established after stipulated time,
        // break out of connection loop
        if (millis() - startTime > connectionTimeout)
            return false;
    }

    // attempt to reconnect on disconnection
    WiFi.onEvent([](WiFiEvent_t wifiEvent, WiFiEventInfo_t wifiEventInfo) {
        ConnectWifi();
    },
                 WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    return true;
};

bool isWifiConnected()
{
    if (WiFi.status() != WL_CONNECTED)
        return false;
    return true;
};

tm getDateTime()
{
    struct tm tmstruct;

    // connect to wifi if not yet connected
    if (WiFi.status() != WL_CONNECTED)
        ConnectWifi();

    // connect to date time server
    configTime(
        3600 * timezone,
        3600 * daysavetime,
        "time.nist.gov",
        "0.pool.ntp.org",
        "1.pool.ntp.org");
    // give some time to complete connection to server
    delay(dateTimeServerResponseTimeout);
    tmstruct.tm_year = 0;

    // retrieve date time
    getLocalTime(&tmstruct, dateTimeServerResponseTimeout);

    return tmstruct;
};

String getCurrentDate()
{
    String dateString = "";
    struct tm tmstruct = getDateTime();

    dateString += String(baseYear + tmstruct.tm_year);
    dateString += GET_TWO_DIGIT_STRING(mthIncrement + tmstruct.tm_mon);
    dateString += GET_TWO_DIGIT_STRING(tmstruct.tm_mday);

    return dateString;
};

String getCurrentTime()
{
    String timeString = "";
    struct tm tmstruct = getDateTime();

    timeString += GET_TWO_DIGIT_STRING(tmstruct.tm_hour);
    timeString += timeDelimiter;
    timeString += GET_TWO_DIGIT_STRING(tmstruct.tm_min);
    timeString += timeDelimiter;
    timeString += GET_TWO_DIGIT_STRING(tmstruct.tm_sec);

    return timeString;
};
