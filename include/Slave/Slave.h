#pragma once

#ifndef SLAVE_H
#define SLAVE_H

#include <Arduino.h>
#include "Slave/Constants.h"
#include "WCS/WCS.h"
#include "Echo/Echo.h"
#include "Logger/Logger.h"
#include "ControlCharacters/ControlCharacters.h"
#include "Slave/Task/Task.h"
#include "Slave/Step/Step.h"
#include "Slave/Step/Constants.h"

using namespace SlaveConstants;
using namespace ControlCharacters;
using namespace StepConstants;

// forward declaration
class WCS;

class SlaveComms
{
public:
    SlaveComms();
    SlaveComms(String, ENUM_SLAVE_ACTIONS, String);
    SlaveComms(ENUM_SLAVE_ACTIONS, String);
    void init(String, ENUM_SLAVE_ACTIONS, String);
    void init(ENUM_SLAVE_ACTIONS, String);
    String toString(bool);
    String toString();

    String getUuid();
    ENUM_SLAVE_ACTIONS getAction();
    String getInstructions();
    int getMsgLength();

private:
    String uuid;
    ENUM_SLAVE_ACTIONS action;
    String instructions;
    int msgLength;
};

class Slave
{
public:
    Slave();
    bool init(HardwareSerial *);
    void setWcsInstance(WCS *);
    void run();

    void updateSlothole();

    void onRetrieveBin(String);
    void onStoreBin(String);
    void onMove(String);
    void onBufferTransfer(String);

private:
    HardwareSerial *ss;
    WCS *wcsInstance;
    EchoBroker echoBroker;

    String serialIn;
    bool isSerialConnected;
    unsigned int lastSerialPingMillis;
    unsigned int lastSlaveResetMillis;

    Task taskManager;
    unsigned int lastBatteryReqMillis;

private:
    bool send(String, bool, bool, unsigned int);
    bool send(String, bool, bool);
    bool send(SlaveComms, bool, bool);
    bool send(SlaveComms, bool);
    bool send(SlaveComms);
    bool send(Step *);

    bool serialRead();
    void extractSerialInput();
    SlaveComms interpret(String);
    void perform(SlaveComms);

    void runPing();

    void startTask();
};

#endif