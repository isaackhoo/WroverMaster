#pragma once

#ifndef STATUS_H
#define STATUS_H

#include "../include/common.h"
#include "../WCS/WCS.h"
#include "../Logger/SD/SD.h"
#include "../Helpers/Local/LocalHelper.h"

// -------------------------
// Status Private Variables
// -------------------------
static const int DEFAULT_ID_LENGTH = 4;
static const char *DEFAULT_ID = "-1-1";

static const int DEFAULT_LEVEL_MIN = 1;
static const int DEFAULT_LEVEL_MAX = 21;
static const char *DEFAULT_LEVEL = "02";
static const char DEFAULT_STATUS_DELIMITER = ',';

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
    bool setActionEnum(String);
    bool setInstructions(String);
    bool setLevel(String);
    bool setPos(String);
    bool setState(ENUM_SHUTTLE_STATE, bool);
    bool setState(ENUM_SHUTTLE_STATE);
    bool setActivityState();
    bool setIsCarryingBin(bool);

    bool setDefault();
    bool setWcsInputs(String, String);
    bool rehydrateStatus();
    void saveStatus();

    String getId();
    String getActionEnum();
    String getInstructions();
    String getLevel();
    String getPos();
    ENUM_SHUTTLE_STATE getState();
    String getStringState();
    bool getIsCarryingBin();
    
    bool isIdDefault();
};
extern Status status;

#endif