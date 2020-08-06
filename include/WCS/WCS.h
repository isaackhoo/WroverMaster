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

using namespace WcsConstants;
using namespace ControlCharacters;
using namespace Helper;

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
    void run();

    bool send(String, bool, bool, unsigned int);
    bool send(String, bool, bool);
    bool send(WcsComms, bool, bool);
    bool send(WcsComms, bool);
    bool send(WcsComms);
    void loginToServer();

private:
    EchoBroker echoBroker;
    bool pong;
    bool pongChecked;
    unsigned int lastPingMillis;
    unsigned int droppedPings;

private:
    void pingServer();
    void runPing();
    void startPings();
    void updatePingReceived();

    void extractTcpInput();
    WcsComms interpret(String);
    void perform(WcsComms);
};

#endif