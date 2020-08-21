#pragma once

#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
#include "Slave/Task/Constants.h"
#include "Slave/Step/Step.h"
#include "Slave/Step/Constants.h"
#include "Status/Status.h"
#include "Status/Constants.h"
#include "Logger/Logger.h"

using namespace TaskConstants;
using namespace StepConstants;

class TaskInterpretor
{
public:
    TaskInterpretor();
    bool interpretStoRetTask(String);

    String getRack();
    String getColumn();
    String getSlothole();

    ENUM_EXTENSION_DEPTH getDepth();
    ENUM_EXTENSION_DIRECTION getDirection();

private:
    String rack;
    String column;
    String slothole;

    ENUM_EXTENSION_DEPTH depth;
    ENUM_EXTENSION_DIRECTION direction;

private:
    String toSlothole(int, int);
};

class Task
{
public:
    Task();
    void init();

    // task creation
    bool createStorageTask(String);
    bool createRetrievalTask(String);
    bool createMovementTask(String);
    bool createBufferTransferTask(String);

    // step handling
    Step *begin();
    Step *validateCurrentStep(String);

private:
    Step *HEAD;
    Step *TAIL;
    Step *iterator;

private:
    Step *concatSteps(Step *, Step *);
    bool appendToList(Step *);
    void reset();

    // micro tasks
    Step *moveTo(String);
    Step *receiveBin(ENUM_EXTENSION_DEPTH, ENUM_EXTENSION_DIRECTION);
    Step *releaseBin(ENUM_EXTENSION_DEPTH, ENUM_EXTENSION_DIRECTION);
    int determineBinSensingDepth(ENUM_EXTENSION_DEPTH);
    int determineBinSensingDeviation(ENUM_EXTENSION_DEPTH);
    int determineEmptyBinSlotSensing(ENUM_EXTENSION_DEPTH);
};

#endif