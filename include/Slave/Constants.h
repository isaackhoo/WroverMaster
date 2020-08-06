#pragma once

#ifndef SLAVE_CONSTANTS_H
#define SLAVE_CONSTANTS_H

#include <Arduino.h>

namespace SlaveConstants
{
    extern const int DEFAULT_SERIAL_BAUD_RATE;

    // actions
    enum ENUM_SLAVE_ACTIONS
    {
        SLAVE_LOGIN = 0,
        SLAVE_RESET,
        SLAVE_ECHO,
        LOG,
        LOGERROR,
        SLAVE_PING,

        ENGAGE_ESTOP,
        DISENGAGE_ESTOP,

        MOVETO, // implicitly uses brake and dual sensors

        READ_BIN_SENSOR,

        EXTEND_ARM,
        HOME_ARM, // implicitly also uses arm sensors

        EXTEND_FINGER_PAIR,
        RETRACT_FINGER_PAIR,

        SLAVE_BATTERY,
        SLAVE_ERROR,
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

    // instructions
    extern const int INST_RACK_ID_LEN;
    extern const int INST_COL_ID_LEN;
    extern const int INST_BINPOS_LEN;

    // rack setup
    extern const int MIN_COLUMN;
    extern const int MAX_COLUMN;
    extern const int MIN_BINSLOT;
    extern const int MAX_BINSLOT;

    // pings
    extern const unsigned long SLAVE_PING_INTERVAL;

    // serial communications
    extern const String HEADER_DELIMITER;
    extern const String BODY_DELIMITER;
}; // namespace SlaveConstants

#endif