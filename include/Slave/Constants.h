#pragma once

#ifndef SLAVE_CONSTANTS_H
#define SLAVE_CONSTANTS_H

#include <Arduino.h>

#define BATTERY_REQ_INTERVAL 60000 // 1 min

namespace SlaveConstants
{
    extern const int DEFAULT_SERIAL_BAUD_RATE;

    // actions
    enum ENUM_SLAVE_ACTIONS
    {
        SLAVE_LOGIN = 0,
        SLAVE_RESET, // 1
        SLAVE_ECHO, // 2
        LOG, // 3
        LOGERROR, // 4
        SLAVE_PING, // 5

        ENGAGE_ESTOP, // 6
        DISENGAGE_ESTOP, // 7

        MOVETO, // 8 implicitly uses brake and dual sensors

        READ_BIN_SENSOR, // 9

        EXTEND_ARM, // 10
        HOME_ARM, // 11 implicitly also uses arm sensors

        EXTEND_FINGER_PAIR, // 12
        RETRACT_FINGER_PAIR, // 13

        SLAVE_BATTERY, // 14
        SLAVE_ERROR, // 15
        UPDATE_SLOTHOLE, // 16
        Num_Slave_Actions_Enums
    };
    const String EnumSlaveActionsString[(int)Num_Slave_Actions_Enums] = {
        "SLAVE_LOGIN",
        "SLAVE_RESET",
        "SLAVE_ECHO",
        "LOG",
        "LOGERROR",
        "SLAVE_PING",

        "ENGAGE_ESTOP",
        "DISENGAGE_ESTOP",

        "MOVETO", // implicitly uses brake and dual sensors

        "READ_BIN_SENSOR",

        "EXTEND_ARM",
        "HOME_ARM", // implicitly also uses arm sensors

        "EXTEND_FINGER_PAIR",
        "RETRACT_FINGER_PAIR",

        "SLAVE_BATTERY",
        "SLAVE_ERROR",
    }; 

    // pings
    extern const unsigned long SLAVE_PING_INTERVAL;

    // serial communications
    extern const String HEADER_DELIMITER;
    extern const String BODY_DELIMITER;
}; // namespace SlaveConstants

#endif