#include "./Status.h"

// -------------------------
// Status Public Variables
// -------------------------
Status status;

// -------------------------
// Status Public Methods
// -------------------------
Status::Status()
{
    this->setDefault();
}

Status::~Status(){};

// Setters
bool Status::setId(String id)
{
    if (id.length() != DEFAULT_ID_LENGTH)
        return false;
    this->id = id;
    return true;
};

bool Status::setActionEnum(String actionEnum)
{
    int aeint = actionEnum.toInt();
    if (aeint < 0 || aeint > Num_Of_WCS_Action_Enums)
        return false;
    this->actionEnum = actionEnum;
    return true;
};

bool Status::setInstructions(String inst)
{
    if (inst.length() <= 0)
        return false;
    this->instructions = inst;
    return true;
};

bool Status::setLevel(String level)
{
    int lvlInt = level.toInt();
    if (lvlInt < DEFAULT_LEVEL_MIN || lvlInt > DEFAULT_LEVEL_MAX)
        return false;
    this->currentLevel = level;
    return true;
};

bool Status::setPos(String pos)
{
    this->currentPos = pos;
    return true;
};

bool Status::setState(ENUM_SHUTTLE_STATE currentState, bool log)
{
    if (currentState < 0 || currentState > Num_Of_Shuttle_States)
        return false;

    if (this->getState() != currentState)
    {
        this->state = currentState;
        String logStr = "Status updated to ";
        logStr += SHUTTLE_STATE_STRING[this->getState()];
        if (log)
        {
            logMasterToSd(logStr);
        }
    }
};

bool Status::setState(ENUM_SHUTTLE_STATE currentState)
{
    this->setState(currentState, true);
};

bool Status::setActivityState()
{
    ENUM_SHUTTLE_STATE activityState = IDLE;
    switch ((ENUM_WCS_ACTIONS)this->actionEnum.toInt())
    {
    case RETRIEVEBIN:
    {
        activityState = RETRIEVING;
        break;
    }
    case STOREBIN:
    {
        activityState = STORING;
        break;
    }
    case MOVE:
    {
        activityState = MOVING;
        break;
    }
    default:
        break;
    }
    this->setState(activityState);
};

bool Status::setIsCarryingBin(bool isCarryingBin)
{
    if (this->isCarryingBin == isCarryingBin)
        return false;
    this->isCarryingBin = isCarryingBin;
    return true;
};

bool Status::setDefault()
{
    this->setId(DEFAULT_ID);
    this->setLevel(DEFAULT_LEVEL);
    this->setState(ENUM_SHUTTLE_STATE::IDLE);
};

bool Status::setWcsInputs(String actionEnum, String inst)
{
    bool res = false;
    res = this->setActionEnum(actionEnum);
    res = this->setInstructions(inst);
    return res;
};

bool Status::rehydrateStatus()
{
    String rehydrator = readStatus();
    if (rehydrator.length() <= 0)
        return false;

    int extractionPos = 0;
    int delimiterIndex = 0;
    int lastExtractionPos = 0;

    delimiterIndex = rehydrator.indexOf(DEFAULT_STATUS_DELIMITER, lastExtractionPos);

    while (delimiterIndex >= 0)
    {
        // extract information
        String token = rehydrator.substring(lastExtractionPos, delimiterIndex);
        switch (extractionPos)
        {
        case 0:
        {
            // shuttle id
            this->setId(token);
            Logger::info("Rehydrated Shuttle Id as " + token);
            break;
        }
        case 1:
        {
            // current level
            this->setLevel(token);
            Logger::info("Rehydrated Shuttle Level to " + token);
            break;
        }
        case 2:
        {
            // state
            this->setState((ENUM_SHUTTLE_STATE)token.toInt());
            String log = "Rehydrated Shuttle State to ";
            log += SHUTTLE_STATE_STRING[this->getState()];
            Logger::info(log);
            break;
        }
        default:
            break;
        }

        // increment extraction pos
        ++extractionPos;
        // update delimiter pos
        if (delimiterIndex + 1 <= rehydrator.length())
        {
            lastExtractionPos = delimiterIndex + 1;
        }
        // search for next delimiter position
        delimiterIndex = rehydrator.indexOf(DEFAULT_STATUS_DELIMITER, lastExtractionPos);
    }
};

void Status::saveStatus()
{
    String statusString = "";
    statusString += this->getId();
    statusString += DEFAULT_STATUS_DELIMITER;
    statusString += this->getLevel();
    statusString += DEFAULT_STATUS_DELIMITER;
    statusString += GET_TWO_DIGIT_STRING(this->getState());

    logStatus(statusString);
};

// Getters
String Status::getId() { return this->id; };
String Status::getActionEnum() { return this->actionEnum; };
String Status::getInstructions() { return this->instructions; };
String Status::getLevel() { return this->currentLevel; };
String Status::getPos() { return this->currentPos; };
ENUM_SHUTTLE_STATE Status::getState() { return this->state; };
String Status::getStringState() { return GET_TWO_DIGIT_STRING(this->state); };
bool Status::getIsCarryingBin() { return this->isCarryingBin; };
bool Status::isIdDefault() { return this->getId() == DEFAULT_ID; };
