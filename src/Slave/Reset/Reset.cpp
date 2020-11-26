#include "Slave/Reset/Reset.h"

// --------------------------------
// RESET PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// RESET PUBLIC METHODS
// --------------------------------

Reset::Reset() : Relay() {};

void Reset::resetSlave()
{
    offSlave();
    delay(2000);
    powerSlave();
};


// --------------------------------
// RESET PRIVATE VARIABLES
// --------------------------------
void Reset::offSlave()
{
    this->relayHH();
};

void Reset::powerSlave()
{
    this->relayLL();
};

// --------------------------------
// RESET PRIVATE METHODS
// --------------------------------