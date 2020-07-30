#pragma once

#ifndef WCS_H
#define WCS_H

#include "common.h"
#include "controlCharacters.h"
#include "WCS/Constants.h"
#include "Status/Status.h"
#include "Logger/Logger.h"
#include "Logger/SD/SD.h"
#include "Network/TCP/TCP.h"
#include "Network/Wifi/WifiNetwork.h"
#include "Helpers/Local/LocalHelper.h"
#include "Helpers/ESP/LocalEsp.h"
#include "Echo/Echo.h"
#include "Slave/Slave.h"

// forward declarations
class Status; 
class Slave;

using namespace Logger;
using namespace WcsConstants;

// --------------------------
// WCS Public Methods
// --------------------------
class WCS
{
private:
    Status *statusInstance;
    Slave *slaveInstance;

    Echo *echos;
    SenderFunction bindedSender;

    bool pong;
    bool pongChecked;
    unsigned int lastPingMillis;
    unsigned int droppedPings;

    String createSendString(WcsCommsFormat);
    String createSendString(WcsCommsFormat, bool);
    bool send(String, bool, bool);
    bool send(WcsCommsFormat, bool, bool);
    bool send(WcsCommsFormat, bool);
    bool send(WcsCommsFormat);

    void pingServer();
    void runPing();
    void startPings();
    void updatePingReceived();

    WcsCommsFormat *interpret(String);
    void perform(WcsCommsFormat *);
    void handleTcpInput();

public:
    WCS();
    bool init(Status *);
    void setSlaveInstance(Slave *);
    void run();

    void updateStateChange();
    void notifyTaskCompletion();
};

#endif