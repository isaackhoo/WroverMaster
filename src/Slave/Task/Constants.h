#pragma once

#ifndef TASKCONSTANTS_H
#define TASKCONSTANTS_H

#include "common.h"

namespace TaskConstants
{
    // direction
    enum ENUM_DIRECTION
    {
        LEFT = -1,
        RIGHT = 1
    };

    // extension
    enum ENUM_EXTENSION_DEPTH
    {
        FIRST_DEPTH = 642,
        SECOND_DEPTH = 1185,
        BUFFER_DEPTH = 900,
    };

    // interpretation
    struct TaskInstructions
    {
        String rack;
        String column;
        String binPos;
        ENUM_EXTENSION_DEPTH depth;
        ENUM_DIRECTION direction;
    };

    // movement
    extern const int MV_SLOTHOLES_PER_COL;
    enum ENUM_AMOVE_SLOTHOLE
    {
        POS_BUFFER = 0,
        POS_IN_LIFTER = -1,
        POS_BATTERY_CHANGE = -2,
        POS_MAINTENANCE = 119
    };
}; // namespace TaskConstants

#endif