#pragma once

#ifndef WCS_CONSTANTS_H
#define WCS_CONSTANTS_H

#include <Arduino.h>

namespace WcsConstants
{
    extern const unsigned int PING_INTERVAL;
    extern const unsigned int PING_DROPPED_DURATION;
    extern const unsigned int MAX_DROPPED_PINGS;

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
    const String EnumWcsActionString[(int)Num_Of_WCS_Action_Enums] = {
        "NONE",
        "LOGIN",       // 1
        "LOGOUT",      // 2
        "PING",        // 3
        "RETRIEVEBIN", // 4
        "STOREBIN",    // 5
        "MOVE",        // 6
        "BATTERY",     // 7
        "STATE",       // 8
        "LEVEL",       // 9
        "SET",         // 10
        "ECHO",        // 11
        "ERROR",       // 12
    };
}; // namespace WcsConstants

#endif