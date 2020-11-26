#pragma once

#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include "Comms/DigitalComms.h"
#include "Slave/Relay/Constants.h"

class Relay : public DigitalComms
{
    public:
    Relay();
    
    void relayHH(); // close both lanes
    void relayLL(); // basically do nothing..
    void relayHL(); // close lane A 
    void relayLH(); // close lane B

    private:
    DigitalComms laneA;
    DigitalComms laneB;
};

#endif