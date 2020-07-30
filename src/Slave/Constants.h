#pragma once

#ifndef SLAVECONSTANTS_H
#define SLAVECONSTANTS_H

namespace SlaveConstants
{
    extern const int DEFAULT_SERIAL_BAUD_RATE;

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
    extern const int INST_RACK_ID_LEN;
    extern const int INST_COL_ID_LEN;
    extern const int INST_BINPOS_LEN;

    // rack setup
    extern const int MIN_COLUMN;
    extern const int MAX_COLUMN;
    extern const int MIN_BINSLOT;
    extern const int MAX_BINSLOT;

    // pings
    extern const unsigned long SLAVE_PING_INTERVAL;        // 10s
    extern const unsigned long SLAVE_PING_DROPPED_DURATION; // 1s after sending out ping
    extern const unsigned int SLAVE_MAX_DROPPED_PINGS;

    // serial communications
    extern const String HEADER_DELIMITER;
    extern const String BODY_DELIMITER;
    class SlaveCommsFormat
    {
    public:
        String uuid;
        int messageLength; // excludes STX and ETX
        String action;
        String instructions;

        SlaveCommsFormat(String uuid, String act, String inst)
        {
            this->uuid = uuid;
            this->action = act;
            this->instructions = inst;
            this->messageLength = this->action.length() + BODY_DELIMITER.length() + this->instructions.length();
        };
        SlaveCommsFormat(String act, String inst) : SlaveCommsFormat(String(millis()), act, inst){};
        SlaveCommsFormat(){};
    };

} // namespace SlaveConstants

#endif