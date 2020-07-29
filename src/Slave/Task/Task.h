#pragma once

#ifndef TASK_H
#define TASK_H

#include "Slave/Step/Step.h"
#include "Status/Status.h"

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

class Task
{
private:
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
    Task();
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