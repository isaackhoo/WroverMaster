#pragma once

#ifndef STATUS_H
#define STATUS_H

#include <Arduino.h>
#include "Status/Constants.h"
#include "Logger/SD/SD.h"
#include "Logger/Logger.h"
#include "Helper/Helper.h"
#include "WCS/Constants.h"

using namespace StatusConstants;
using namespace Logger;
using namespace Helper;
using namespace WcsConstants;

namespace Status
{
    bool init();
    void saveStatus();

    // setters
    bool setLevel(String);
    bool setActionEnum(ENUM_WCS_ACTIONS);
    bool setInstructions(String);
    bool setState(ENUM_SHUTTLE_STATE, bool);
    bool setState(ENUM_SHUTTLE_STATE);
    bool setSlothole(String);
    void setTask(ENUM_WCS_ACTIONS, String);
    void clearNotification(); 
    
    // getters
    String getId();
    String getLevel();
    String getActionEnum();
    String getInstructions();
    ENUM_SHUTTLE_STATE getState();
    String getStateString();
    String getSlothole();

    bool isIdDefault();
    bool shouldNotifyStatusChange();

}; // namespace Status

#endif