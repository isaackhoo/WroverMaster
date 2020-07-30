#pragma once

#ifndef TASK_H
#define TASK_H

#include "Slave/Task/Constants.h"
#include "Slave/Step/Step.h"
#include "Status/Status.h"

// forward declaration
class Status;

using namespace TaskConstants;

class Task
{
private:
    Status *statusInstance;
    Step *HEAD;
    Step *TAIL;
    Step *iterator;

    Step *concatSteps(Step *, Step *);
    void appendList(Step *);
    void reset();

    // helper
    TaskInstructions *interpretTaskInstructions(String);
    String toSlothole(int, int);

    // micro tasks
    Step *moveTo(String);
    Step *receiveBin(ENUM_EXTENSION_DEPTH, ENUM_DIRECTION);
    Step *releaseBin(ENUM_EXTENSION_DEPTH, ENUM_DIRECTION);

public:
    Task(Status *);
    ~Task();

    // getters
    Step *getCurrentStep();

    // Macro tasks
    bool createStorageTask(String);
    bool createRetrievalTask(String);
    bool createMovementTask(String);
    bool createBufferTransferTask();

    // methods
    Step *begin();
    Step *validateStep(String);
};

#endif