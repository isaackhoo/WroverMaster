#include "./Status.h"

// -------------------------
// Status Private Variables
// -------------------------
const int DEFAULT_ID_LENGTH = 4;
const char *DEFAULT_ID = "-1-1";

const int DEFAULT_LEVEL_MIN = 1;
const int DEFAULT_LEVEL_MAX = 21;
const char *DEFAULT_LEVEL = "02";
const char DEFAULT_STATUS_DELIMITER = ',';

// -------------------------
// Status Public Variables
// -------------------------
Status *status = new Status();

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

void Status::clearActionEnum()
{
    this->actionEnum = "";
}

bool Status::setInstructions(String inst)
{
    if (inst.length() <= 0)
        return false;
    this->instructions = inst;
    return true;
};

void Status::clearInstructions()
{
    this->instructions = "";
}

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
    return true;
};

bool Status::setState(ENUM_SHUTTLE_STATE currentState)
{
    return this->setState(currentState, true);
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
    return this->setState(activityState);
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
    bool res = true;
    if (res)
        res = this->setId(DEFAULT_ID);
    if (res)
        res = this->setLevel(DEFAULT_LEVEL);
    if (res)
        res = this->setState(ENUM_SHUTTLE_STATE::IDLE);
    return res;
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

    bool res = false;
    int extractionCase = 0;
    int delimiterIndex = 0;
    int lastExtractionPos = 0;

    delimiterIndex = rehydrator.indexOf(DEFAULT_STATUS_DELIMITER, lastExtractionPos);
    if (delimiterIndex == -1)
    {
        // no delimiter found
        return res;
    }

    // process tokens
    while (lastExtractionPos < rehydrator.length())
    {
        // extract information
        String token = rehydrator.substring(lastExtractionPos, delimiterIndex);

        switch (extractionCase)
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
        case 3:
        {
            // shuttle position
            this->setPos(token);
            Logger::info("Rehydrated Shuttle Position to " + this->getPos());
            res = true; // last extraction completed. May not contain action and instructions
            break;
        }
        case 4:
        {
            // action enum
            this->setActionEnum(token);
            Logger::info("Rehydrated actionEnum " + token);
            res = false; // there should be instructions if action exists
            break;
        }
        case 5:
        {
            // instructions
            this->setInstructions(token);
            Logger::info("Rehydrated instructions " + token);
            res = true; // last extraction completed
            break;
        }
        default:
            break;
        }

        // increment extraction case
        ++extractionCase;
        // update next token search starting position
        lastExtractionPos = delimiterIndex + 1;
        // search for next delimiter position
        delimiterIndex = rehydrator.indexOf(DEFAULT_STATUS_DELIMITER, lastExtractionPos);
        if (delimiterIndex == -1)
            delimiterIndex = rehydrator.length();
    }
    return res;
};

void Status::saveStatus()
{
    String statusString = "";
    statusString += this->getId();
    statusString += DEFAULT_STATUS_DELIMITER;
    statusString += this->getLevel();
    statusString += DEFAULT_STATUS_DELIMITER;
    statusString += GET_TWO_DIGIT_STRING(this->getState());
    statusString += DEFAULT_STATUS_DELIMITER;
    statusString += this->getPos();
    if (this->getActionEnum() != "")
    {
        statusString += DEFAULT_STATUS_DELIMITER;
        statusString += this->getActionEnum();
        statusString += DEFAULT_STATUS_DELIMITER;
        statusString += this->getInstructions();
    }

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
