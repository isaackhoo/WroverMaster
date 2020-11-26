#pragma once

#ifndef RESET_H
#define RESET_H

#include <Arduino.h>
#include "Slave/Relay/Relay.h"

class Reset : public Relay
{
    public:
    Reset();
    
    void resetSlave();
    
    private:
    void offSlave(); // close both lanes
    void powerSlave(); // basically do nothing..

    private:
};

#endif