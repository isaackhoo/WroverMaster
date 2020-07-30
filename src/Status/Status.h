#pragma once

#ifndef STATUS_H
#define STATUS_H

#include "common.h"
#include "Status/Constants.h"
#include "WCS/WCS.h"
#include "WCS/Constants.h"
#include "Logger/Logger.h"
#include "Logger/SD/SD.h"
#include "Helpers/Local/LocalHelper.h"

class WCS; // forward decare WCS

using namespace Logger;
using namespace StatusConstants;
using namespace WcsConstants;

// -------------------------
// Status Public Methods
// -------------------------
class Status
{
private:
    WCS *wcsInstance;

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
    void setWcsContext(WCS *);

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

#endif