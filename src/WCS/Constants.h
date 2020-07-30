#pragma once

#ifndef WCSCONSTANTS_H
#define WCSCONSTANTS_H

namespace WcsConstants
{
    extern const unsigned long PING_INTERVAL;        // 10s
    extern const unsigned long PING_DROPPED_DURATION; // 1s after sending out ping
    extern const unsigned int MAX_DROPPED_PINGS;

    extern const unsigned long ECHO_TIMEOUT_DURATION; // 5s

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
        MANUAL_SET_DEFAULT = 0,
        MANUAL_SET_ID,    // 01
        MANUAL_SET_LEVEL, // 02
        MANUAL_SET_STATE, // 03
        MANUAL_SET_ECHO,  // 04
        Num_Of_Manual_Set_Types_Enums
    };

    extern const int DEFAULT_ENUM_VALUE_LENGTH;

    struct WcsCommsFormat
    {
        String id;           // shuttle 4 digit ID
        String actionEnum;   // 2 digit action enum
        String instructions; // variable length string
    };
} // namespace WcsConstants

#endif