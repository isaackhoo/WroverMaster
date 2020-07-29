#include "Task.h"

// movement
const int MV_SLOTHOLES_PER_COL = 6;

// --------------------------
// Task Private Methods
// --------------------------
Step *Task::concatSteps(Step *first, Step *next)
{
    if (first == NULL || next == NULL)
        return NULL;

    first->setNext(next);
    next->setPrev(first);

    return next;
};

void Task::appendList(Step *step)
{
    // is first node
    if (this->HEAD == NULL && this->TAIL == NULL)
    {
        this->HEAD = step;
        this->TAIL = step;
    }
    // append to existing list
    else
        this->concatSteps(this->TAIL, step);

    // iterate TAIL to end of list
    while (this->TAIL->getNext() != NULL)
        this->TAIL = this->TAIL->getNext();
};

void Task::reset()
{
    while (this->HEAD != NULL)
    {
        this->iterator = this->HEAD;
        this->HEAD = this->HEAD->getNext();
        delete this->iterator;
    }
    this->TAIL = NULL;
};

// helper
TaskInstructions *Task::interpretTaskInstructions(String inst)
{
    // validate instructions
    if (inst.length() != (INST_RACK_ID_LEN + INST_COL_ID_LEN + INST_BINPOS_LEN))
        return NULL;

    int fromIdx = 0;
    TaskInstructions *taskInst = new TaskInstructions();
    taskInst->rack = inst.substring(fromIdx, fromIdx + INST_RACK_ID_LEN);
    fromIdx += INST_RACK_ID_LEN;
    taskInst->column = inst.substring(fromIdx, fromIdx + INST_COL_ID_LEN);
    fromIdx += INST_COL_ID_LEN;
    taskInst->binPos = inst.substring(fromIdx, fromIdx + INST_BINPOS_LEN);

    // interpret depth and direction from instructions
    taskInst->depth = taskInst->binPos.toInt() % 2 == 0 ? SECOND_DEPTH : FIRST_DEPTH;
    taskInst->direction = taskInst->rack.toInt() % 2 == 0 ? RIGHT : LEFT;

    return taskInst;
};

String Task::toSlothole(int col, int binPos)
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

// ------------------------------------ micro tasks ---------------------------------------
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! USE WITH CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Creates loose pointers to be returned for further use.
// If not handled, will cause memory overflow
// ----------------------------------------------------------------------------------------
Step *Task::moveTo(String slothole)
{
    bool utilizeEStop = false;
    if (status->getPos().toInt() == POS_IN_LIFTER || slothole.toInt() == POS_IN_LIFTER)
        utilizeEStop = true;

    Step *disengageEstop = utilizeEStop ? new Step(DISENGAGE_ESTOP, 0) : NULL;
    Step *move = new Step(MOVETO, slothole.toDouble());
    this->concatSteps(disengageEstop, move);
    Step *engageEstop = utilizeEStop ? new Step(ENGAGE_ESTOP, 1) : NULL;
    this->concatSteps(move, engageEstop);

    Step *firstStep = utilizeEStop ? disengageEstop : move;

    return firstStep;
};

Step *Task::receiveBin(ENUM_EXTENSION_DEPTH depth, ENUM_DIRECTION direction)
{
    Step *init = new Step(EXTEND_ARM, depth * direction);
    Step *next = this->concatSteps(init, new Step(EXTEND_FINGER_PAIR, direction));
    next = this->concatSteps(next, new Step(HOME_ARM, 0));
    next = this->concatSteps(next, new Step(RETRACT_FINGER_PAIR, direction));

    return init;
};

Step *Task::releaseBin(ENUM_EXTENSION_DEPTH depth, ENUM_DIRECTION direction)
{
    double reverse = -1;

    Step *init = new Step(EXTEND_FINGER_PAIR, reverse * direction);
    Step *next = this->concatSteps(init, new Step(EXTEND_ARM, depth * direction));
    next = this->concatSteps(next, new Step(HOME_ARM, 0));
    next = this->concatSteps(next, new Step(RETRACT_FINGER_PAIR, reverse * direction));

    return init;
};

// --------------------------
// Task Public Methods
// --------------------------
Task::Task()
{
    this->HEAD = NULL;
    this->TAIL = NULL;
    this->iterator = NULL;
};

Task::~Task()
{
    this->reset();
};

Step *Task::getCurrentStep() { return this->iterator; };

bool Task::createStorageTask(String storageInst)
{
    // interpret instructions
    TaskInstructions *ti = this->interpretTaskInstructions(storageInst);
    if (ti == NULL)
        return false;

    // convert storage instructions to movement position value
    String storageSlothole = this->toSlothole(ti->column.toInt(), ti->binPos.toInt());

    // create storage task
    this->reset();
    this->appendList(this->moveTo(String(POS_BUFFER)));
    this->appendList(this->receiveBin(BUFFER_DEPTH, LEFT));
    this->appendList(this->moveTo(storageSlothole));
    this->appendList(this->releaseBin(ti->depth, ti->direction));

    delete ti;

    return true;
};

bool Task::createRetrievalTask(String retrievalInstructions)
{
    // interpret instructions
    TaskInstructions *ti = this->interpretTaskInstructions(retrievalInstructions);
    if (ti == NULL)
        return false;

    // convert retrieval instructions to movement position value
    String retrievalSlothole = this->toSlothole(ti->column.toInt(), ti->binPos.toInt());

    // create retrieval task
    this->reset();
    this->appendList(this->moveTo(retrievalSlothole));
    this->appendList(this->receiveBin(ti->depth, ti->direction));
    this->appendList(this->moveTo(String(POS_BUFFER)));
    this->appendList(this->releaseBin(BUFFER_DEPTH, RIGHT));

    delete ti;

    return true;
};

bool Task::createMovementTask(String inst)
{
    // determine which admin position to move to
    String adminSlothole = "";
    switch ((ENUM_AMOVE_SLOTHOLE)inst.toInt())
    {
    case POS_BUFFER:
    {
        adminSlothole = String(POS_BUFFER);
        break;
    }
    case POS_IN_LIFTER:
    {
        adminSlothole = String(POS_IN_LIFTER);
        break;
    }
    case POS_BATTERY_CHANGE:
    {
        adminSlothole = String(POS_BATTERY_CHANGE);
        break;
    }
    case POS_MAINTENANCE:
    {
        adminSlothole = String(POS_MAINTENANCE);
        break;
    }
    default:
        break;
    }

    if (adminSlothole == "")
        return false;

    this->reset();
    this->appendList(this->moveTo(adminSlothole));

    return true;
};

bool Task::createBufferTransferTask()
{
    this->reset();
    this->appendList(this->moveTo(String(POS_BUFFER)));
    this->appendList(this->receiveBin(BUFFER_DEPTH, LEFT));
    this->appendList(this->releaseBin(BUFFER_DEPTH, RIGHT));

    return true;
};

Step *Task::begin()
{
    this->iterator = this->HEAD;
    this->iterator->setStatus(STEP_ACTIVE);
    return this->iterator;
};

Step *Task::validateStep(String result)
{
    bool res = this->iterator->evaluate(result);
    if (res)
    {
        // step completed successfully
        this->iterator->setStatus(STEP_COMPLETED);
        // move iterator to next step
        this->iterator = this->iterator->getNext();
        if (this->iterator != NULL)
            this->iterator->setStatus(STEP_ACTIVE);
    }

    return this->iterator;
};