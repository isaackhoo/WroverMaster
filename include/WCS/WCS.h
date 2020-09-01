#pragma once

#ifndef WCS_H
#define WCS_H

#include "Logger/Logger.h"
#include "Helper/Helper.h"
#include "Status/Status.h"
#include "Status/Constants.h"
#include "ControlCharacters/ControlCharacters.h"
#include "WCS/Constants.h"
#include "WCS/TCP/TCP.h"
#include "Echo/Echo.h"
#include "ESPWrover/ESPWrover.h"
#include "Slave/Slave.h"

using namespace WcsConstants;
using namespace ControlCharacters;
using namespace Helper;

// forward declaration
class Slave;

class WcsComms
{
public:
    WcsComms();
    WcsComms(ENUM_WCS_ACTIONS, String);
    void init(ENUM_WCS_ACTIONS, String);
    String toString(bool);
    String toString();

    ENUM_WCS_ACTIONS getActionEnum();
    String getInstructions();

private:
    ENUM_WCS_ACTIONS actionEnum;
    String instructions;
};

class WCS
{
public:
    WCS();
    bool init();
    void setSlaveInstance(Slave *);
    void run();

    void loginToServer();
    void notifyTaskCompletion(String);
    void notifyTaskCompletion();
    void notifyTaskFailure(String);
    void updateBatteryLevel(String);

private:
    Slave *slaveInstance;
    EchoBroker echoBroker;
    bool pong;
    bool pongChecked;
    unsigned int lastPingMillis;
    unsigned int droppedPings;

    WcsComms *completionNotification;

private:
    bool send(String, bool, bool, unsigned int);
    bool send(String, bool, bool);
    bool send(WcsComms, bool, bool);
    bool send(WcsComms, bool);
    bool send(WcsComms);

    void pingServer();
    void runPing();
    void startPings();
    void updatePingReceived();

    void extractTcpInput();
    WcsComms interpret(String);
    void perform(WcsComms);
};

#endif