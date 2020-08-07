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

Step::Step(ENUM_SLAVE_ACTIONS action, double target, double deviation, String inst) : Step()
{
    this->stepAction = action;
    this->stepStatus = STEP_AWAITING_START;
    this->stepTarget = target;
    this->fixedDeviation = deviation;
    this->instructions.reserve(128);
    this->instructions = inst;
};

Step::Step(ENUM_SLAVE_ACTIONS a, double t, String i) : Step(a, t, 0, i){};

Step::Step(ENUM_SLAVE_ACTIONS a, double t) : Step(a, t, ""){};

// setters
bool Step::setStatus(ENUM_STEP_STATUS status)
{
    if (status == this->getStepStatus())
        return false;
    this->stepStatus = status;
    return true;
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
    double lowerBound = this->getStepTarget() - this->getStepFixedDeviation();
    double upperBound = this->getStepTarget() + this->getStepFixedDeviation();

    return lowerBound >= val && val <= upperBound;
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
