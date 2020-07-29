#pragma once

#ifndef SLAVECONSTANTS_H
#define SLAVECONSTANTS_H

namespace SlaveConstants
{
    const int DEFAULT_SERIAL_BAUD_RATE = 115200;

    // actions
    enum ENUM_SLAVE_ACTIONS
    {
        SLAVE_ECHO = 0,
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
    };

    // instructions
    const int INST_RACK_ID_LEN = 2;
    const int INST_COL_ID_LEN = 2;
    const int INST_BINPOS_LEN = 2;

    // rack setup
    const int MIN_COLUMN = -2;
    const int MAX_COLUMN = 20;
    const int MIN_BINSLOT = 1;
    const int MAX_BINSLOT = 12;

} // namespace SlaveConstants

#endif