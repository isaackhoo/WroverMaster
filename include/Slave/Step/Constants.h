#pragma once

#ifndef STEP_CONSTANTS_H
#define STEP_CONSTANTS_H

#include <Arduino.h>

#define DEFAULT_MAX_RETRIES 100

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

    enum ENUM_COMPARISON_TYPE
    {
        CMP_DEFAULT = 0,
        CMP_GREATER_THAN,
        CMP_LESS_THAN
    };

}; // namespace StepConstants

#endif