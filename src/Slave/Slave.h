#pragma once

#ifndef SLAVE_H
#define SLAVE_H

#include "common.h"
#include "controlCharacters.h"
#include <HardwareSerial.h>
#include "Slave/Constants.h"
#include "Task/Task.h"
#include "Echo/Echo.h"
#include "Helpers/ESP/LocalEsp.h"
#include "Step/Step.h"
#include "Status/Status.h"
#include "WCS/WCS.h"

// forward declaration
class Status;
class Task;
class WCS;

using namespace Logger;
using namespace SlaveConstants;

// --------------------------
// Slave Public Methods
// --------------------------
class Slave
{
private:
    HardwareSerial *ss;
    Status *statusInstance;
    WCS *wcsInstance;
    Echo *echos;
    SenderFunction bindedSender;

    bool pong;
    bool pongChecked;
    unsigned int lastPingMillis;
    unsigned int droppedPings;

    Task *taskManager;

    String createSendString(SlaveCommsFormat, bool);
    String createSendString(SlaveCommsFormat);
    bool serialRead();
    String serialIn;
    bool send(String, bool, bool);
    bool send(SlaveCommsFormat, bool, bool);
    bool send(SlaveCommsFormat, bool);
    bool send(SlaveCommsFormat);
    bool send(Step *);

    void pingSlaveChip();
    void runPing();
    void startPings();
    void updatePingReceived();

    SlaveCommsFormat *interpret(String);
    void perform(SlaveCommsFormat *);
    void handleSerialInput();

    void updateStatus(String, String);

public:
    Slave();
    bool init(HardwareSerial *, Status *);
    void setWcsInstance(WCS *);
    void run();

    void onRetrieveBin(String);
    void onStoreBin(String);
    void onMove(String);
    void onBattery();
};

#endif