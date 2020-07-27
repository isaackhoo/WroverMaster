#pragma once

#ifndef WCS_H
#define WCS_H

#include "../include/common.h"
#include "./Logger/Logger.h"
#include "./Logger/SD/SD.h"
#include "./Network/TCP/TCP.h"
#include "./Network/Wifi/WifiNetwork.h"
#include "./Status/Status.h"
#include "./Helpers/Local/LocalHelper.h"
#include "./Helpers/ESP/LocalEsp.h"

using namespace Logger;

// --------------------------
// WCS Private Variables
// --------------------------
static const char *SOH = "\x01";
static const char *STX = "\x02";
static const char *ETX = "\x03";
static const char *EOT = "\x04";

static const unsigned long PING_INTERVAL = 1000 * 30;         // 30s
static const unsigned long PING_DROPPED_INTERVAL = 1000 * 35; // 35s

static const unsigned long ECHO_TIMEOUT_DURATION = 1000 * 5; // 5s

enum ENUM_WCS_ACTIONS
{
    NONE = 0,
    LOGIN,       // 1
    LOGOUT,      // 2
    PING,        // 3
    RETRIEVEBIN, // 4
    STOREBIN,    // 5
    MOVE,        // 6
    BATTERY,     // 7
    STATE,       // 8
    LEVEL,       // 9
    SET,         // 10
    ECHO,        // 11
    ERROR,       // 12
    Num_Of_WCS_Action_Enums
};

enum ENUM_MANUAL_SET_TYPES
{
    SET_DEFAULT = 0,
    SET_ID,    // 01
    SET_LEVEL, // 02
    SET_STATE, // 03
    SET_ECHO,  // 04
    Num_Of_Manual_Set_Types_Enums
};

const int DEFAULT_ENUM_VALUE_LENGTH = 2;

struct WcsCommsFormat {
    String id; // shuttle 4 digit ID
    String actionEnum; // 2 digit action enum
    String instructions; // variable length string
};

// --------------------------
// WCS Public Variables
// --------------------------

// --------------------------
// WCS Public Methods
// --------------------------
class WCS
{
private:
    bool send(WcsCommsFormat, bool, bool);
    bool send(WcsCommsFormat, bool);
    bool send(WcsCommsFormat);

public:
    WCS();
    void init();
    void run();
    bool updateStateChange();
};

// export only a single instance of WCS
extern WCS wcs;

#endif