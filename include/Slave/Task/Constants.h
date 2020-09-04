#pragma once

#ifndef TASK_CONSTANTS_H
#define TASK_CONSTANTS_H

#include <Arduino.h>

#define ARM_EXTENSION_TOLERANCE 5

namespace TaskConstants
{
    // Arm
    enum ENUM_EXTENSION_DIRECTION
    {
        LEFT = -1,
        RIGHT = 1,
        Num_Extension_Directions = 2
    };
    const String EnumExtensionDirectionString[(int)Num_Extension_Directions] = {
        "LEFT",
        "RIGHT",
    };

    enum ENUM_EXTENSION_DEPTH
    {
        FIRST_DEPTH = 642,
        SECOND_DEPTH = 1185,
        BUFFER_DEPTH = 900,
        OFFSET = 30,
        HOME_DEPTH = 0,
        Num_Extension_Depths = 4
    };

    // Bin sensor
    enum ENUM_BIN_SENSOR_DISTANCES
    {
        BS_DEPTH_BUFFER = 50,
        BS_DEPTH_FIRST = 30,
        BS_DEPTH_SECOND = 105,
    };

    enum ENUM_BIN_SENSOR_DEVIATION
    {
        BS_SD_BUFFER = 10,
        BS_SD_FIRST = 10,
        BS_SD_SECOND = 15,
    };

    // E-Stop
    enum ENUM_ESTOP_EXTENSION
    {
        ESTOP_EXTEND = 635,
        ESTOP_RETRACT = 30,
        ESTOP_DEVIATION = 10,
        Num_Estop_Extensions = 2
    };

    // Movememnt
    enum ENUM_MOVEMENT_DIRECTION
    {
        FORWARD = 1,
        REVERSE = -1,
        Num_Movement_Directions = 2
    };
    const String EnumMovementDirectionString[(int)Num_Movement_Directions] = {
        "FORWARD",
        "REVERSE",
    };

    extern const int MV_SLOTHOLES_PER_COL;
    enum ENUM_AMOVE_SLOTHOLE
    {
        SLOTHOLE_BUFFER = 0,
        SLOTHOLE_IN_LIFTER = -1,
        SLOTHOLE_BATTERY_CHANGE = -2,
        SLOTHOLE_MAINTENANCE = 119,
        Num_Amove_Slotholes = 4
    };
    const String EnumAmoveSlotholeString[(int)Num_Amove_Slotholes] = {
        "SLOTHOLE_BUFFER",
        "SLOTHOLE_IN_LIFTER",
        "SLOTHOLE_BATTERY_CHANGE",
        "SLOTHOLE_MAINTENANCE",
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

}; // namespace TaskConstants

#endif