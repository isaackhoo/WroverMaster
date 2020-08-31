#pragma once

#ifndef STEP_H
#define STEP_H

#include <Arduino.h>
#include "Slave/Step/Constants.h"
#include "Slave/Constants.h"
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"

using namespace StepConstants;
using namespace SlaveConstants;
using namespace Helper;
using namespace ControlCharacters;

class Step
{
public:
    Step *prev;
    Step *next;

public:
    Step();
    Step(ENUM_SLAVE_ACTIONS, double, double, String, ENUM_COMPARISON_TYPE, unsigned int);
    Step(ENUM_SLAVE_ACTIONS, double, double, String, ENUM_COMPARISON_TYPE);
    Step(ENUM_SLAVE_ACTIONS, double, String);
    Step(ENUM_SLAVE_ACTIONS, double, double, ENUM_COMPARISON_TYPE, unsigned int);
    Step(ENUM_SLAVE_ACTIONS, double, double, ENUM_COMPARISON_TYPE);
    Step(ENUM_SLAVE_ACTIONS, double, double);
    Step(ENUM_SLAVE_ACTIONS, double, ENUM_COMPARISON_TYPE, unsigned int);
    Step(ENUM_SLAVE_ACTIONS, double, ENUM_COMPARISON_TYPE);
    Step(ENUM_SLAVE_ACTIONS, double);

    // setters
    bool setStatus(ENUM_STEP_STATUS);

    unsigned int incrementRetries();
    bool setErrorDetails(String);

    // getters
    ENUM_SLAVE_ACTIONS getStepAction();
    ENUM_STEP_STATUS getStepStatus();
    double getStepTarget();
    double getStepFixedDeviation();
    String getStepInstructions();

    unsigned int getMaxRetries();
    unsigned int getCurrentRetries();
    String getStepErrorDetails();

    String toString();

    // validation
    bool validate(double);
    bool validate(String);

private:
    ENUM_SLAVE_ACTIONS stepAction;
    ENUM_STEP_STATUS stepStatus;
    double stepTarget;
    double fixedDeviation;
    String instructions;
    ENUM_COMPARISON_TYPE cmpType;

    unsigned int maxRetries;
    unsigned int retries;
    String errorDetails;
};

#endif