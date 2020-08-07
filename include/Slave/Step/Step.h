#pragma once

#ifndef STEP_H
#define STEP_H

#include <Arduino.h>
#include "Slave/Step/Constants.h"
#include "Slave/Constants.h"

using namespace StepConstants;
using namespace SlaveConstants;

class Step
{
public:
    Step();
    Step(ENUM_SLAVE_ACTIONS, double, double, Strin)
};

#endif