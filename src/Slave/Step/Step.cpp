#include "Slave/Step/Step.h"

// -------------------------------
// STEP PUBLIC VARIABLES
// -------------------------------

// -------------------------------
// STEP PUBLIC METHODS
// -------------------------------
Step::Step()
{
    this->prev = NULL;
    this->next = NULL;
    this->errorDetails.reserve(128);
};

Step::Step(ENUM_SLAVE_ACTIONS action, double target, double deviation, String inst, ENUM_COMPARISON_TYPE type, unsigned int maxRetries) : Step()
{
    this->stepAction = action;
    this->stepStatus = STEP_AWAITING_START;
    this->stepTarget = target;
    this->fixedDeviation = deviation;
    this->instructions.reserve(128);
    this->instructions = inst;
    this->cmpType = type;

    this->maxRetries = maxRetries;
    this->retries = 0;
};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, double d, String i, ENUM_COMPARISON_TYPE type) : Step(a, t, d, i, type, DEFAULT_MAX_RETRIES){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, String i) : Step(a, t, 0, i, CMP_DEFAULT){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, double d, ENUM_COMPARISON_TYPE type, unsigned int r) : Step(a, t, d, "", type, r){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, double d, ENUM_COMPARISON_TYPE type) : Step(a, t, d, "", type){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, double d) : Step(a, t, d, CMP_DEFAULT){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, ENUM_COMPARISON_TYPE type, unsigned int r) : Step(a, t, 0, type, r){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, ENUM_COMPARISON_TYPE type) : Step(a, t, 0, type){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t) : Step(a, t, CMP_DEFAULT){};

// setters
bool Step::setStatus(ENUM_STEP_STATUS status)
{
    if (status == this->getStepStatus())
        return false;
    this->stepStatus = status;
    return true;
};

unsigned int Step::incrementRetries()
{
    this->retries += 1;
    return this->getCurrentRetries();
};

bool Step::setErrorDetails(String err)
{
    if (err.length() <= 0)
        return false;
    this->errorDetails += err;
    return true;
};

// getters
ENUM_SLAVE_ACTIONS Step::getStepAction() { return this->stepAction; };
ENUM_STEP_STATUS Step::getStepStatus() { return this->stepStatus; };
double Step::getStepTarget() { return this->stepTarget; };
double Step::getStepFixedDeviation() { return this->fixedDeviation; };
String Step::getStepInstructions() { return this->getStepInstructions(); };

unsigned int Step::getMaxRetries() { return this->maxRetries; };
unsigned int Step::getCurrentRetries() { return this->retries; };
String Step::getStepErrorDetails() { return this->errorDetails; };

String Step::toString()
{
    String str((char *)0);
    str.reserve(128);

    str += "[";
    str += EnumStepStatusString[(int)this->getStepStatus()];
    str += "] ";
    str += EnumSlaveActionsString[(int)this->getStepAction()];
    str += " - ";
    str += String(this->getStepTarget());
    str += " - ";
    str += this->getStepInstructions();

    return str;
};

// validation
bool Step::validate(double val)
{
    bool res = false;

    double lowerBound = this->getStepTarget() - this->getStepFixedDeviation();
    double upperBound = this->getStepTarget() + this->getStepFixedDeviation();

    switch (this->cmpType)
    {
    case CMP_DEFAULT:
    {
        // checks that value falls within bounds
        res = lowerBound <= val && val <= upperBound;
        break;
    }
    case CMP_GREATER_THAN:
    {
        // checks that value is greater than upper bound
        res = val > upperBound;
        break;
    }
    case CMP_LESS_THAN:
    {
        // checks that value is less than lower bound
        res = val < lowerBound;
        break;
    }
    default:
        break;
    }

    return res;
};

bool Step::validate(String res)
{
    if (res.startsWith(String(NAK)))
    {
        this->setStatus(STEP_ERROR);
        this->setErrorDetails(res.substring(1)); // ignore NAK char
        return false;
    }
    else
        return this->validate(res.toDouble());
};

// -------------------------------
// STEP PRIVATE VARIABLES
// -------------------------------

// -------------------------------
// STEP PRIVATE METHODS
// -------------------------------
