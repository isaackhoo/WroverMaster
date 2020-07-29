#pragma once

#ifndef SLAVE_H
#define SLAVE_H

#include "common.h"
#include "controlCharacters.h"
#include <HardwareSerial.h>
#include "Constants.h"
#include "Task/Task.h"
#include "Echo/Echo.h"
#include "Helpers/ESP/LocalEsp.h"
#include "Step/Step.h"
#include "Status/Status.h"
#include "WCS/WCS.h"

using namespace Logger;
using namespace SlaveConstants;

// pings
static const unsigned long SLAVE_PING_INTERVAL = 1000 * 10;        // 10s
static const unsigned long SLAVE_PING_DROPPED_DURATION = 1000 * 1; // 1s after sending out ping
static const unsigned int SLAVE_MAX_DROPPED_PINGS = 3;

// serial communications
static const String HEADER_DELIMITER = ",";
static const String BODY_DELIMITER = "-";
class SlaveCommsFormat
{
public:
    String uuid;
    int messageLength; // excludes STX and ETX
    String action;
    String instructions;

    SlaveCommsFormat(String uuid, String act, String inst)
    {
        this->uuid = uuid;
        this->action = act;
        this->instructions = inst;
        this->messageLength = this->action.length() + BODY_DELIMITER.length() + this->instructions.length();
    };
    SlaveCommsFormat(String act, String inst) : SlaveCommsFormat(String(millis()), act, inst){};
    SlaveCommsFormat(){};
};

class Task; // forward linking Task

// --------------------------
// Slave Public Methods
// --------------------------
class Slave
{
private:
    HardwareSerial *ss;
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
    bool init(HardwareSerial *);
    void run();

    void onRetrieveBin(String);
    void onStoreBin(String);
    void onMove(String);
    void onBattery();
};
extern Slave *slave;

#endif