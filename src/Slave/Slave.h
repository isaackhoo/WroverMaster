#pragma once

#ifndef SLAVE_H
#define SLAVE_H

#include "../include/common.h"
#include "../include/controlCharacters.h"
#include <HardwareSerial.h>
#include "./Constants.h"
#include "../Echo/Echo.h"

using namespace Logger;

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

    String createSendString(SlaveCommsFormat, bool);
    String createSendString(SlaveCommsFormat);
    bool serialRead();
    String serialIn;
    bool send(String, bool, bool);
    bool send(SlaveCommsFormat, bool, bool);
    bool send(SlaveCommsFormat, bool);
    bool send(SlaveCommsFormat);

    void pingSlaveChip();
    void runPing();
    void startPings();

    void handleSerialInput();

public:
    Slave();
    bool init(HardwareSerial *);
    void run();
};
extern Slave *slave;

#endif