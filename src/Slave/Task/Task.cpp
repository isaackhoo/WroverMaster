#include "Slave/Task/Task.h"

namespace TaskConstants
{
    // Movememnt
    const int MV_SLOTHOLES_PER_COL = 6;

    // instructions
    const int INST_RACK_ID_LEN = 2;
    const int INST_COL_ID_LEN = 2;
    const int INST_BINPOS_LEN = 2;

    // rack setup
    const int MIN_COLUMN = -2;
    const int MAX_COLUMN = 20;
    const int MIN_BINSLOT = 1;
    const int MAX_BINSLOT = 12;

}; // namespace TaskConstants

// --------------------------------------------------- TASK INTERPRETOR ---------------------------------------------------
// ---------------------------------
// TASKINTERPRETOR PUBLIC VARIABLES
// ---------------------------------

// ---------------------------------
// TASKINTERPRETOR PUBLIC METHODS
// ---------------------------------
TaskInterpretor::TaskInterpretor()
{
    this->rack.reserve(4);
    this->column.reserve(4);
    this->slothole.reserve(4);
};

bool TaskInterpretor::interpretStoRetTask(String inst)
{
    // validate instructions
    if (inst.length() != (INST_RACK_ID_LEN + INST_COL_ID_LEN + INST_BINPOS_LEN))
        return false;

    // extract information
    int fromIdx = 0;
    this->rack += inst.substring(fromIdx, fromIdx + INST_RACK_ID_LEN);
    fromIdx += INST_RACK_ID_LEN;
    this->column += inst.substring(fromIdx, fromIdx + INST_COL_ID_LEN);
    fromIdx += INST_COL_ID_LEN;
    int binPos = inst.substring(fromIdx, fromIdx + INST_BINPOS_LEN).toInt();

    // calculate slothole to travel to
    this->slothole = this->toSlothole(this->getColumn().toInt(), binPos);

    // interpret depth and direction for arm extensions
    this->depth = binPos % 2 == 0 ? SECOND_DEPTH : FIRST_DEPTH;
    this->direction = this->getRack().toInt() % 2 == 0 ? RIGHT : LEFT;

    return true;
};

String TaskInterpretor::getRack() { return this->rack; };
String TaskInterpretor::getColumn() { return this->column; };
String TaskInterpretor::getSlothole() { return this->slothole; };
ENUM_EXTENSION_DEPTH TaskInterpretor::getDepth() { return this->depth; };
ENUM_EXTENSION_DIRECTION TaskInterpretor::getDirection() { return this->direction; };

// ---------------------------------
// TASKINTERPRETOR PRIVATE VARIABLES
// ---------------------------------

// ---------------------------------
// TASKINTERPRETOR PRIVATE METHODS
// ---------------------------------
String TaskInterpretor::toSlothole(int col, int binPos)
{
    // calculates the position of the bin relative to buffer position 0
    // buffer is a column 0
    // first bin column starts at 1.
    // gap between column and bin is different from gap between bin and bin

    // calcualte bin distance to move within column
    // note that distance of first bin from column is different
    //      |               |   |               |   |
    //      |               |   |               |   |
    //      |               |   |               |   |
    //      |      Bin      |   |      Bin      |   |
    //      |               |   |               |   |
    //      |               |   |               |   |
    //      | ---  400 ---  |   |  --- 400 ---  |   |
    //-100 -|_______________| 80|_______________| 80|__ ...

    // get just the bin column
    // since bins are position and numbered as such
    //

    //  02   04   06   08   10   12
    //
    //  01   03   05   07   09   11

    String res = "";
    bool isValid = true;

    // validate inputs
    if (col < MIN_COLUMN || MAX_COLUMN < col)
        isValid = false;
    if (binPos < MIN_BINSLOT || MAX_BINSLOT < binPos)
        isValid = false;

    if (isValid)
    {
        if (col <= 0)
            res = String(col);
        else
        {
            int colToShc = (col - 1) * MV_SLOTHOLES_PER_COL;
            int binCol = ((binPos + 1) / 2); // (1) + 1 / 2 = 1; (2) + 1 / 2 = 1; (3) + 1 / 2 = 2 etc..
            res = String(colToShc + binCol);
        }
    }

    return res;
};

// --------------------------------------------------- TASK ---------------------------------------------------
// ----------------------------
// TASK PUBLIC VARIABLES
// ----------------------------

// ----------------------------
// TASK PUBLIC METHODS
// ----------------------------
Task::Task()
{
    this->init();
};

void Task::init()
{
    this->HEAD = NULL;
    this->TAIL = NULL;
    this->iterator = NULL;
};

// task creation
bool Task::createStorageTask(String storageInst)
{
    bool res = true;
    // interpret instructions
    TaskInterpretor storageInterpretor;
    res = storageInterpretor.interpretStoRetTask(storageInst);

    if (res)
    {
        // create storage task
        this->reset();
        this->appendToList(this->moveTo(String(SLOTHOLE_BUFFER)));
        this->appendToList(this->receiveBin(BUFFER_DEPTH, LEFT)); // receive from storage buffer
        this->appendToList(this->moveTo(storageInterpretor.getSlothole()));
        this->appendToList(this->releaseBin(storageInterpretor.getDepth(), storageInterpretor.getDirection()));
    }

    return res;
};

bool Task::createRetrievalTask(String retrievalInst)
{
    bool res = true;
    // interpret instructions
    TaskInterpretor retrievalInterpretor;
    res = retrievalInterpretor.interpretStoRetTask(retrievalInst);

    if (res)
    {
        // create retrieval task
        this->reset();
        this->appendToList(this->moveTo(retrievalInterpretor.getSlothole()));
        this->appendToList(this->receiveBin(retrievalInterpretor.getDepth(), retrievalInterpretor.getDirection()));
        this->appendToList(this->moveTo(String(SLOTHOLE_BUFFER)));
        this->appendToList(this->releaseBin(BUFFER_DEPTH, RIGHT));
    };

    return res;
};

bool Task::createMovementTask(String slothole)
{
    int slotholeInt = slothole.toInt();
    if (slotholeInt < StatusConstants::DEFAULT_SLOTHOLE_MIN || slotholeInt > StatusConstants::DEFAULT_SLOTHOLE_MAX)
    {
        return false;
    }

    this->reset();
    return this->appendToList(this->moveTo(slothole));
};

bool Task::createBufferTransferTask(String fromBuffer)
{
    ENUM_EXTENSION_DIRECTION fromDirection = (ENUM_EXTENSION_DIRECTION)fromBuffer.toInt();
    if (fromDirection != LEFT && fromDirection != RIGHT)
        return false;

    ENUM_EXTENSION_DIRECTION to = fromDirection == LEFT ? RIGHT : LEFT;

    // create transfer task
    this->reset();
    this->appendToList(this->receiveBin(BUFFER_DEPTH, fromDirection));
    this->appendToList(this->releaseBin(BUFFER_DEPTH, to));

    return true;
};

bool Task::createReceiveTask(String wordedInst)
{
    int res = this->interpretWordedInstructions(wordedInst);
    Logger::log("receive: " + String(res));
    ENUM_EXTENSION_DIRECTION direction = res < 0 ? ENUM_EXTENSION_DIRECTION::LEFT : ENUM_EXTENSION_DIRECTION::RIGHT;
    // create receive task
    this->reset();
    this->appendToList(this->receiveBin((ENUM_EXTENSION_DEPTH)abs(res), direction));

    return true;
};

bool Task::createReleaseTask(String wordedInst)
{
    int res = this->interpretWordedInstructions(wordedInst);
    Logger::log("release: " + String(res));
    ENUM_EXTENSION_DIRECTION direction = res < 0 ? ENUM_EXTENSION_DIRECTION::LEFT : ENUM_EXTENSION_DIRECTION::RIGHT;
    // create release task
    this->reset();
    this->appendToList(this->releaseBin((ENUM_EXTENSION_DEPTH)abs(res), direction));

    return true;
};

// step handling
Step *Task::begin()
{
    this->iterator = this->HEAD;
    if (this->iterator != NULL)
        this->iterator->setStatus(STEP_ACTIVE);
    return this->iterator;
};

Step *Task::validateCurrentStep(String val)
{
    if (this->iterator == NULL)
        return NULL;

    bool res = this->iterator->validate(val);
    if (res)
    {
        // check if it was a movement step. update slothole count if it was
        if (this->iterator->getStepAction() == MOVETO)
        {
            Status::setSlothole(val);
            Status::saveStatus();
        }

        // step completed successfully
        this->iterator->setStatus(STEP_COMPLETED);
        // move iterator to next step
        this->iterator = this->iterator->next;
        if (this->iterator != NULL)
            this->iterator->setStatus(STEP_ACTIVE);
    }
    else
    {
        // check if step has already errored out
        if (this->iterator->getStepStatus() != ENUM_STEP_STATUS::STEP_ERROR)
        {
            // step has just sent back feedback that is out of bounds.
            // retry step
            this->iterator->incrementRetries();
            if (this->iterator->getCurrentRetries() > this->iterator->getMaxRetries())
            {
                this->iterator->setStatus(ENUM_STEP_STATUS::STEP_ERROR);
                this->iterator->setErrorDetails("Max step retries reached");
            }
        }
    }

    return this->iterator;
};

// ----------------------------
// TASK PRIVATE VARIABLES
// ----------------------------

// ----------------------------
// TASK PRIVATE METHODS
// ----------------------------
Step *Task::concatSteps(Step *first, Step *next)
{
    if (first == NULL || next == NULL)
        return NULL;

    first->next = next;
    next->prev = first;

    return next;
};

bool Task::appendToList(Step *step)
{
    if (step == NULL)
        return false;

    // is first node
    if (this->HEAD == NULL && this->TAIL == NULL)
    {
        this->HEAD = step;
        this->TAIL = step;
    }
    // append to existing list
    else
        this->concatSteps(this->TAIL, step);

    // iterate tail to end of list
    while (this->TAIL->next != NULL)
        this->TAIL = this->TAIL->next;

    return true;
};

void Task::reset()
{
    while (this->HEAD != NULL)
    {
        this->iterator = this->HEAD;
        this->HEAD = this->HEAD->next;
        delete this->iterator;
        this->iterator = NULL;
    };
    this->TAIL = NULL;
};

// micro tasks
Step *Task::moveTo(String slothole)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // creates and returns loose pointers
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    bool useEStop = false;

    // determine if move task should include the use of estop
    if ((ENUM_AMOVE_SLOTHOLE)Status::getSlothole().toInt() == SLOTHOLE_IN_LIFTER || (ENUM_AMOVE_SLOTHOLE)slothole.toInt() == SLOTHOLE_IN_LIFTER)
        useEStop = true;

    // Step *positionEstop = useEStop
    //                           ? new Step(DISENGAGE_ESTOP, ESTOP_RETRACT, ESTOP_DEVIATION)
    //                           : new Step(ENGAGE_ESTOP, ESTOP_EXTEND, ESTOP_DEVIATION);
    // // ensure arm is homed
    // Step *next = this->concatSteps(positionEstop, new Step(HOME_ARM, HOME_DEPTH, ARM_EXTENSION_TOLERANCE));
    // // DO NOT ALLOW RETRY..fucking hell. shuttle ran mad before, and took out an Estop barrier -.-|||
    // next = this->concatSteps(next, new Step(MOVETO, slothole.toDouble(), CMP_DEFAULT, 0));
    // Step *extendEstop = useEStop
    //                         ? new Step(ENGAGE_ESTOP, ESTOP_EXTEND, ESTOP_DEVIATION)
    //                         : NULL;
    // if (useEStop)
    //     this->concatSteps(next, extendEstop);

    // return positionEstop;

    Step *next = new Step(MOVETO, slothole.toDouble(), CMP_DEFAULT, 0);

    return next;
};

Step *Task::receiveBin(ENUM_EXTENSION_DEPTH depth, ENUM_EXTENSION_DIRECTION direction)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // creates and returns loose pointers
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    double reverse = -1;

    // check if bin is in slot
    Step *init = new Step(READ_BIN_SENSOR, this->determineBinSensingDepth(depth) * direction, this->determineBinSensingDeviation(depth));
    // extend arm and finger
    Step *next = this->concatSteps(init, new Step(EXTEND_ARM, depth * direction, ARM_EXTENSION_TOLERANCE));
    next = this->concatSteps(next, new Step(EXTEND_FINGER_PAIR, direction));
    // offset retrieval to prevent finger jam
    next = this->concatSteps(next, new Step(EXTEND_ARM, reverse * direction * OFFSET, ARM_EXTENSION_TOLERANCE));
    // home arm
    next = this->concatSteps(next, new Step(HOME_ARM, HOME_DEPTH, ARM_EXTENSION_TOLERANCE));
    next = this->concatSteps(next, new Step(RETRACT_FINGER_PAIR, direction));
    // check that bin slot is now empty - bin has been successfully retrieved
    next = this->concatSteps(next, new Step(READ_BIN_SENSOR, this->determineEmptyBinSlotSensing(depth) * direction, this->getComparisonType(direction)));

    return init;
};

Step *Task::releaseBin(ENUM_EXTENSION_DEPTH depth, ENUM_EXTENSION_DIRECTION direction)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // creates and returns loose pointers
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    double reverse = -1;

    // check that no bin exist at slot
    Step *init = new Step(READ_BIN_SENSOR, this->determineEmptyBinSlotSensing(depth) * direction, this->getComparisonType(direction));
    // extend fingers and arm to release bin
    Step *next = this->concatSteps(init, new Step(EXTEND_FINGER_PAIR, reverse * direction));
    next = this->concatSteps(next, new Step(EXTEND_ARM, depth * direction, ARM_EXTENSION_TOLERANCE));
    // home arm
    next = this->concatSteps(next, new Step(HOME_ARM, HOME_DEPTH, ARM_EXTENSION_TOLERANCE));
    next = this->concatSteps(next, new Step(RETRACT_FINGER_PAIR, reverse * direction));
    // check that bin is in place
    next = this->concatSteps(next, new Step(READ_BIN_SENSOR, this->determineBinSensingDepth(depth) * direction, this->determineBinSensingDeviation(depth)));

    return init;
};

int Task::determineBinSensingDepth(ENUM_EXTENSION_DEPTH depth)
{
    int expectedScanValue = 0;
    switch (depth)
    {
    case BUFFER_DEPTH:
    {
        expectedScanValue = BS_DEPTH_BUFFER;
        break;
    }
    case FIRST_DEPTH:
    {
        expectedScanValue = BS_DEPTH_FIRST;
        break;
    }
    case SECOND_DEPTH:
    {
        expectedScanValue = BS_DEPTH_SECOND;
        break;
    }
    default:
        break;
    }

    return expectedScanValue;
};

int Task::determineBinSensingDeviation(ENUM_EXTENSION_DEPTH depth)
{
    int expectedStdDev = 0;
    switch (depth)
    {
    case BUFFER_DEPTH:
    {
        expectedStdDev = BS_SD_BUFFER;
        break;
    }
    case FIRST_DEPTH:
    {
        expectedStdDev = BS_SD_FIRST;
        break;
    }
    case SECOND_DEPTH:
    {
        expectedStdDev = BS_SD_SECOND;
        break;
    }
    default:
        break;
    }

    return expectedStdDev;
};

int Task::determineEmptyBinSlotSensing(ENUM_EXTENSION_DEPTH depth)
{
    return this->determineBinSensingDepth(depth) + this->determineBinSensingDeviation(depth);
};

ENUM_COMPARISON_TYPE Task::getComparisonType(ENUM_EXTENSION_DIRECTION direction)
{
    ENUM_COMPARISON_TYPE cmpType = CMP_DEFAULT;

    switch (direction)
    {
    case LEFT:
    {
        cmpType = CMP_LESS_THAN;
        break;
    }
    case RIGHT:
    {
        cmpType = CMP_GREATER_THAN;
        break;
    }
    default:
        break;
    }

    return cmpType;
};

int Task::interpretWordedInstructions(String wordedInst)
{
    Logger::log(wordedInst);

    int res = 0;

    // get depth
    if (wordedInst.indexOf("buffer") >= 0 || wordedInst.indexOf("b") >= 0)
    {
        res = ENUM_EXTENSION_DEPTH::BUFFER_DEPTH;
    }
    else if (wordedInst.indexOf("first") >= 0 || wordedInst.indexOf("f") >= 0)
    {
        res = ENUM_EXTENSION_DEPTH::FIRST_DEPTH;
    }
    else if (wordedInst.indexOf("second") >= 0 || wordedInst.indexOf("s") >= 0)
    {
        res = ENUM_EXTENSION_DEPTH::SECOND_DEPTH;
    };

    // get direction
    if (wordedInst.indexOf("left") >= 0 || wordedInst.indexOf("-") >= 0)
    {
        res *= ENUM_EXTENSION_DIRECTION::LEFT;
    }
    else
    {
        res = abs(res);
    };

    return res;
};