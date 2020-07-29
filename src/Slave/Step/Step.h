#pragma once

#ifndef STEP_H
#define STEP_H

#include "common.h"
#include "Slave/Constants.h"
#include "controlCharacters.h"

using namespace SlaveConstants;

enum ENUM_STEP_STATUS
{
    STEP_AWAITING_START = 0,
    STEP_ACTIVE,
    STEP_COMPLETED,
    STEP_ERROR,
    Num_Of_Step_Status
};

class Step
{
private:
    ENUM_SLAVE_ACTIONS stepAction;
    ENUM_STEP_STATUS stepStatus;
    double stepTarget;
    double acceptableDeviation;
    String instructions;
    String errorDetails;

    Step *prev;
    Step *next;

public:
    Step(ENUM_SLAVE_ACTIONS, double, double, String);
    Step(ENUM_SLAVE_ACTIONS, double, String);
    Step(ENUM_SLAVE_ACTIONS, double);

    // setters
    bool setStatus(ENUM_STEP_STATUS);
    void setErrorDetails(String);
    void setPrev(Step *);
    void setNext(Step *);

    // getters
    ENUM_SLAVE_ACTIONS getStepAction();
    ENUM_STEP_STATUS getStepStatus();
    double getStepTarget();
    double getAcceptableDeviation();
    String getStepInstructions();
    String getStepErrorDetails();

    String toString();

    Step *getPrev();
    Step *getNext();

    // methods
    bool evaluate(double);
    bool evaluate(String);
};

#endif
