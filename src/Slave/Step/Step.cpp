#include "Step.h"

Step::Step(ENUM_SLAVE_ACTIONS stepAction, double stepTarget, double acceptableDeviation, String instructions)
{
    this->stepAction = stepAction;
    this->stepStatus = STEP_AWAITING_START;
    this->stepTarget = stepTarget;
    this->acceptableDeviation = acceptableDeviation;
    this->instructions = instructions || "";
    this->errorDetails = "";
    this->prev = NULL;
    this->next = NULL;
};

Step::Step(ENUM_SLAVE_ACTIONS stepAction, double stepTarget, String instructions) : Step(stepAction, stepTarget, 0, instructions){};

Step::Step(ENUM_SLAVE_ACTIONS stepAction, double stepTarget) : Step(stepAction, stepTarget, ""){};

bool Step::setStatus(ENUM_STEP_STATUS status)
{
    if (this->getStepStatus() != status)
    {
        this->stepStatus = status;
        return true;
    }
    return false;
};

void Step::setErrorDetails(String err)
{
    this->errorDetails = err;
};

void Step::setPrev(Step *prev)
{
    this->prev = prev;
};

void Step::setNext(Step *next)
{
    this->next = next;
};

ENUM_SLAVE_ACTIONS Step::getStepAction()
{
    return this->stepAction;
};

ENUM_STEP_STATUS Step::getStepStatus()
{
    return this->stepStatus;
};

double Step::getStepTarget()
{
    return this->stepTarget;
};

double Step::getAcceptableDeviation()
{
    return this->acceptableDeviation;
};

String Step::getStepInstructions()
{
    return this->instructions;
};

String Step::getStepErrorDetails()
{
    return this->errorDetails;
};

String Step::toString()
{
    return GET_TWO_DIGIT_STRING(this->getStepAction()) + this->getStepInstructions();
};

Step *Step::getPrev()
{
    return this->prev;
};

Step *Step::getNext()
{
    return this->next;
};

bool Step::evaluate(double val)
{
    double lowerBound = this->getStepTarget() - this->getAcceptableDeviation();
    double upperBound = this->getStepTarget() + this->getAcceptableDeviation();

    return lowerBound >= val && val <= upperBound;
};

bool Step::evaluate(String val)
{
    // check if string contains error character
    if (val.startsWith(NAK))
    {
        this->setStatus(STEP_ERROR);
        this->setErrorDetails(val.substring(1));
        return false;
    }
    else {
        return this->evaluate(val.toDouble());
    }
};