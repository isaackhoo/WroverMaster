#pragma once

#ifndef TASK_CONSTANTS_H
#define TASK_CONSTANTS_H

#include <Arduino.h>

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
        Num_Extension_Depths = 3
    };
    const String EnumExtensionDepthString[(int)Num_Extension_Depths] = {
        "FIRST_DEPTH",
        "SECOND_DEPTH",
        "BUFFER_DEPTH",
    };

    // E-Stop
    enum ENUM_ESTOP_EXTENSION
    {
        ESTOP_EXTEND = 1,
        ESTOP_RETRACT = 0,
        Num_Estop_Extensions = 2
    };
    const String EnumEstopExtensionsString[(int)Num_Estop_Extensions] = {
        "ESTOP_EXTEND",
        "ESTOP_RETRACT",
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