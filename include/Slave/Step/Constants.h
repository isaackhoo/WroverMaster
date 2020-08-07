#pragma once

#ifndef STEP_CONSTANTS_H
#define STEP_CONSTANTS_H

#include <Arduino.h>

namespace StepConstants
{
    enum ENUM_STEP_STATUS
    {
        STEP_AWAITING_START = 0,
        STEP_ACTIVE,    // 1
        STEP_COMPLETED, // 2
        STEP_ERROR,     // 3
        Num_Of_Step_Status
    };
    const String EnumStepStatusString[(int)Num_Of_Step_Status] = {
        "STEP_AWAITING_START",
        "STEP_ACTIVE",    // 1
        "STEP_COMPLETED", // 2
        "STEP_ERROR",     // 3
    };
}; // namespace StepConstants

#endif