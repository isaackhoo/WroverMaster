#pragma once

#ifndef STATUS_H
#define STATUS_H

#include "common.h"
#include "WCS/WCS.h"
#include "Logger/SD/SD.h"
#include "Helpers/Local/LocalHelper.h"

// -------------------------
// Status Private Variables
// -------------------------
extern const int DEFAULT_ID_LENGTH;
extern const char *DEFAULT_ID;

extern const int DEFAULT_LEVEL_MIN;
extern const int DEFAULT_LEVEL_MAX;
extern const char *DEFAULT_LEVEL;
extern const char DEFAULT_STATUS_DELIMITER;

// -------------------------
// Status Public Variables
// -------------------------
enum ENUM_SHUTTLE_STATE
{
    IDLE = 0,
    STORING,       // 01
    RETRIEVING,    // 02
    MOVING,        // 03
    SHUTTLE_ERROR, // 04
    AWAITING,      // 05
    Num_Of_Shuttle_States
};
static const char SHUTTLE_STATE_STRING[][15] = {"IDLE", "STORING", "RETRIEVING", "MOVING", "ERROR", "AWAITING"};

// -------------------------
// Status Public Methods
// -------------------------
class Status
{
private:
    String id;
    String actionEnum;
    String instructions;
    String currentLevel;
    String currentPos;
    ENUM_SHUTTLE_STATE state;

    // hardware status
    bool isCarryingBin;

public:
    Status();
    ~Status();
    bool setId(String);
    bool setLevel(String);
    bool setState(ENUM_SHUTTLE_STATE, bool);
    bool setState(ENUM_SHUTTLE_STATE);
    bool setActionEnum(String);
    void clearActionEnum();
    bool setInstructions(String);
    void clearInstructions();
    bool setPos(String);
    bool setActivityState();
    bool setIsCarryingBin(bool);

    bool setDefault();
    bool setWcsInputs(String, String);
    bool rehydrateStatus();
    void saveStatus();

    String getId();
    String getLevel();
    ENUM_SHUTTLE_STATE getState();
    String getStringState();
    String getActionEnum();
    String getInstructions();
    String getPos();
    bool getIsCarryingBin();

    bool isIdDefault();
};
extern Status *status;

#endif