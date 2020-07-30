#pragma once

#ifndef ECHO_H
#define ECHO_H

#include "common.h"
#include "Echo/Constants.h"

using SenderFunction = std::function<bool(String, bool, bool)>;
using namespace EchoConstants;

struct EchoNode
{
    String uuid;
    unsigned int timeSent;
    int droppedEchoCounter;

    int messageLength;
    String message;

    EchoNode *prev;
    EchoNode *next;
};

class Echo
{
private:
    EchoNode *HEAD;
    EchoNode *TAIL;
    unsigned int totalEchos;

    unsigned int echoTimeoutDuration;
    unsigned int maxDrops;

    void incrementTotalEchos();
    void decrementTotalEchos();

public:
    Echo();
    void setTimeout(unsigned int);
    void setMaxDrops(unsigned int);
    void pushEcho(String, unsigned int, int, String);
    bool removeEcho(String);
    bool verifyEcho(String);
    int getTotalEchos();
    void reset();
    void run(void (*)(String), SenderFunction);
};

#endif