#pragma once

#ifndef STATUS_CONSTANTS_H
#define STATUS_CONSTANTS_H

#include <Arduino.h>

namespace StatusConstants
{
    extern const int DEFAULT_ID_LENGTH;
    extern const String DEFAULT_ID;

    extern const int DEFAULT_LEVEL_MIN;
    extern const int DEFAULT_LEVEL_MAX;
    extern const String DEFAULT_LEVEL;
    extern const int DEFAULT_SLOTHOLE_MIN;
    extern const int DEFAULT_SLOTHOLE_MAX;
    extern const String DEFAULT_SLOTHOLE;
    extern const String DEFAULT_STATUS_DELIMITER;

    // -------------------------
    // Status Public Variables
    // -------------------------
    enum ENUM_SHUTTLE_STATE
    {
        IDLE = 0,
        STORING,       // 01
        RETRIEVING,    // 02
        MOVING,        // 03
        SHUTTLE_ERROR, // 04
        AWAITING,      // 05
        Num_Of_Shuttle_States
    };
    const String EnumShuttleStateString[(int)Num_Of_Shuttle_States] = {
        "IDLE",
        "STORING",       // 01
        "RETRIEVING",    // 02
        "MOVING",        // 03
        "SHUTTLE_ERROR", // 04
        "AWAITING",      // 05
    };

}; // namespace StatusConstants

#endif