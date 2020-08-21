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
    int fromBufferInt = fromBuffer.toInt();
    if (fromBufferInt != LEFT && fromBufferInt != RIGHT)
        return false;

    ENUM_EXTENSION_DIRECTION from = (ENUM_EXTENSION_DIRECTION)fromBufferInt;
    ENUM_EXTENSION_DIRECTION to = from == LEFT ? RIGHT : LEFT;

    // create transfer task
    this->reset();
    this->appendToList(this->receiveBin(BUFFER_DEPTH, from));
    this->appendToList(this->releaseBin(BUFFER_DEPTH, to));

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

    // TODO :: remove
    useEStop = false;

    Step *retractEstop = useEStop ? new Step(DISENGAGE_ESTOP, ESTOP_RETRACT) : NULL;
    Step *move = new Step(MOVETO, slothole.toDouble());
    if (useEStop)
        this->concatSteps(retractEstop, move);
    Step *extendEstop = useEStop ? new Step(ENGAGE_ESTOP, ESTOP_EXTEND) : NULL;
    if (useEStop)
        this->concatSteps(move, extendEstop);

    Step *firstStep = useEStop ? retractEstop : move;

    return firstStep;
};

Step *Task::receiveBin(ENUM_EXTENSION_DEPTH depth, ENUM_EXTENSION_DIRECTION direction)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // creates and returns loose pointers
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Step *init = new Step(EXTEND_ARM, depth * direction, ARM_EXTENSION_TOLERANCE);
    Step *next = this->concatSteps(init, new Step(EXTEND_FINGER_PAIR, direction));
    next = this->concatSteps(next, new Step(HOME_ARM, HOME_DEPTH, ARM_EXTENSION_TOLERANCE));
    next = this->concatSteps(next, new Step(RETRACT_FINGER_PAIR, direction));

    return init;
};

Step *Task::releaseBin(ENUM_EXTENSION_DEPTH depth, ENUM_EXTENSION_DIRECTION direction)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // creates and returns loose pointers
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    double reverse = -1;

    Step *init = new Step(EXTEND_FINGER_PAIR, reverse * direction);
    Step *next = this->concatSteps(init, new Step(EXTEND_ARM, depth * direction, ARM_EXTENSION_TOLERANCE));
    next = this->concatSteps(next, new Step(RETRACT_FINGER_PAIR, reverse * direction));
    next = this->concatSteps(next, new Step(HOME_ARM, HOME_DEPTH, ARM_EXTENSION_TOLERANCE));

    return init;
};