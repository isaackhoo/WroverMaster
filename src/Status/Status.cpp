#include "Status/Status.h"

namespace StatusConstants
{
    const int DEFAULT_ID_LENGTH = 4;
    const String DEFAULT_ID(F("-1-1"));

    const int DEFAULT_LEVEL_MIN = 1;
    const int DEFAULT_LEVEL_MAX = 21;
    const String DEFAULT_LEVEL(F("02"));
    const int DEFAULT_SLOTHOLE_MIN = -2;
    const int DEFAULT_SLOTHOLE_MAX = 119;
    const String DEFAULT_SLOTHOLE(F("0"));
    const String DEFAULT_STATUS_DELIMITER(F(","));
}; // namespace StatusConstants

namespace Status
{
    // ---------------------------
    // STATUS PRIVATE VARIABLES
    // ---------------------------
    namespace
    {
        String _id((char *)0);
        String _level((char *)0);
        String _actionEnum((char *)0);
        String _instructions((char *)0);
        ENUM_SHUTTLE_STATE _state = IDLE;
        String _slothole((char *)0);
        bool _shouldNotify = false;

    }; // namespace

    // ---------------------------
    // STATUS PRIVATE METHODS
    // ---------------------------

    // ---------------------------
    // STATUS PUBLIC VARIABLES
    // ---------------------------

    // ---------------------------
    // STATUS PUBLIC METHODS
    // ---------------------------
    bool setId(String id)
    {
        _id = id;
        return true;
    };

    bool init()
    {
        _id.reserve(5);
        _id = DEFAULT_ID;
        _level.reserve(3);
        _level = DEFAULT_LEVEL;
        _slothole.reserve(4);
        _slothole = DEFAULT_SLOTHOLE;
        _actionEnum.reserve(3);
        _actionEnum = "";
        _instructions.reserve(32);
        _instructions = "";

        String rehydrator((char *)0);
        rehydrator.reserve(256);
        rehydrator += SDCard::readStatus();

        if (rehydrator.length() <= 0)
            return false;

        // break down rehydration string
        bool res = false;
        int extractionCase = 0;
        int delimiterIdx = 0;
        int lastExtractionPos = 0;
        String rehydrateLog((char *)0);
        rehydrateLog.reserve(128);

        delimiterIdx = rehydrator.indexOf(DEFAULT_STATUS_DELIMITER, lastExtractionPos);
        if (delimiterIdx == -1)
            return false;

        // process tokens
        while (lastExtractionPos < rehydrator.length())
        {
            // extract information
            String token(rehydrator.substring(lastExtractionPos, delimiterIdx));
            switch (extractionCase)
            {
            case 0:
            {
                // shuttle id
                setId(token);
                rehydrateLog = F("Rehydrated shuttle id as ");
                rehydrateLog += getId();
                break;
            }
            case 1:
            {
                // current level
                setLevel(token);
                rehydrateLog = F("Rehydrated shuttle level to ");
                rehydrateLog += getLevel();
                break;
            }
            case 2:
            {
                // state
                setState((ENUM_SHUTTLE_STATE)token.toInt(), false);
                rehydrateLog = F("Rehydrated shuttle state to ");
                rehydrateLog += getStateString();
                break;
            }
            case 3:
            {
                // shuttle slothole
                setSlothole(token);
                rehydrateLog = F("Rehydrated shuttle slothole to ");
                rehydrateLog += getSlothole();
                res = true; // may not contain action and instructions in status
                break;
            }
            case 4:
            {
                // shuttle action enum
                setActionEnum((ENUM_WCS_ACTIONS)token.toInt());
                rehydrateLog = F("Rehydrated actionEnum to ");
                rehydrateLog += getActionEnum();
                res = false;
                break;
            }
            case 5:
            {
                // shuttle instructions
                setInstructions(token);
                rehydrateLog = F("Rehydrated instructions ");
                rehydrateLog += getInstructions();
                break;
            }
            default:
                break;
            }

            // log rehydration
            log(rehydrateLog);

            // increment extraction case
            ++extractionCase;
            // update next token search starting position
            lastExtractionPos = delimiterIdx + 1;
            // search for next delimiter position
            delimiterIdx = rehydrator.indexOf(DEFAULT_STATUS_DELIMITER, lastExtractionPos);
            if (delimiterIdx == -1)
                delimiterIdx = rehydrator.length();
        };

        return res;
    };

    void saveStatus()
    {
        String ss((char *)0);
        ss.reserve(128);

        ss += getId();
        ss += DEFAULT_STATUS_DELIMITER;
        ss += getLevel();
        ss += DEFAULT_STATUS_DELIMITER;
        ss += getState();
        ss += DEFAULT_STATUS_DELIMITER;
        ss += getSlothole();
        if (getActionEnum() != "")
        {
            ss += DEFAULT_STATUS_DELIMITER;
            ss += getActionEnum();
            ss += DEFAULT_STATUS_DELIMITER;
            ss += getInstructions();
        }

        SDCard::setStatus(ss);
    };

    // setters
    bool setLevel(String lvl)
    {
        int lvlInt = lvl.toInt();
        if (lvlInt < DEFAULT_LEVEL_MIN || lvlInt > DEFAULT_LEVEL_MAX)
            return false;
        _level = lvl;
        return true;
    };

    bool setActionEnum(ENUM_WCS_ACTIONS action)
    {
        if (action < 0 || action > Num_Of_WCS_Action_Enums)
            return false;
        _actionEnum = GET_TWO_DIGIT_STRING((int)action);

        // determine the corresponding shuttle state
        ENUM_SHUTTLE_STATE activity = IDLE;
        switch (action)
        {
        case STOREBIN:
        {
            activity = STORING;
            break;
        }
        case RETRIEVEBIN:
        {
            activity = RETRIEVING;
            break;
        }
        case MOVE:
        {
            activity = MOVING;
            break;
        }
        default:
            break;
        };

        setState(activity);

        return true;
    };

    bool setInstructions(String inst)
    {
        if (inst.length() <= 0)
            return false;
        _instructions = inst;
        return true;
    };

    bool setState(ENUM_SHUTTLE_STATE state, bool shouldNotify)
    {
        if (state < 0 || state > Num_Of_Shuttle_States)
            return false;

        if (state != getState())
        {
            _state = state;
            if (shouldNotify)
                _shouldNotify = true;
            return true;
        }
        return false;
    };

    bool setState(ENUM_SHUTTLE_STATE state)
    {
        return setState(state, true);
    };

    bool setSlothole(String slothole)
    {
        int shi = slothole.toInt();
        if (shi < DEFAULT_SLOTHOLE_MIN || shi > DEFAULT_SLOTHOLE_MAX)
            return false;
        _slothole = slothole;
        return true;
    };

    void setTask(ENUM_WCS_ACTIONS actionEnum, String inst)
    {
        setActionEnum(actionEnum);
        setInstructions(inst);
        saveStatus();
    };

    void clearTask()
    {
        setActionEnum(NONE);
        setInstructions("");
        saveStatus();
    };

    void clearNotification()
    {
        _shouldNotify = false;
    };

    // getters
    String getId() { return _id; };
    String getLevel() { return _level; };
    String getActionEnum() { return _actionEnum; };
    String getInstructions() { return _instructions; };
    ENUM_SHUTTLE_STATE getState() { return _state; };
    String getStateString() { return EnumShuttleStateString[(int)_state]; };
    String getSlothole() { return _slothole; };

    bool isIdDefault() { return _id == DEFAULT_ID ? true : false; };
    bool shouldNotifyStatusChange() { return _shouldNotify; };
}; // namespace Status
