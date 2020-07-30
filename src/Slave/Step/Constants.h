#pragma once

#ifndef STEPCONSTANTS_H
#define STEPCONSTANTS_H

namespace StepConstants
{
    enum ENUM_STEP_STATUS
    {
        STEP_AWAITING_START = 0,
        STEP_ACTIVE,
        STEP_COMPLETED,
        STEP_ERROR,
        Num_Of_Step_Status
    };
};

#endif