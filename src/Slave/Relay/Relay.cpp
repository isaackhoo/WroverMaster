#include "Slave/Relay/Relay.h"

// --------------------------------
// RELAY PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// RELAY PUBLIC METHODS
// --------------------------------
Relay::Relay()
{
    this->laneA = DigitalComms(TWELVE_VOLT_SLAVE_POWER_CTRL, OUTPUT);
    this->laneB = DigitalComms(TWENTY_FOUR_VOLT_SLAVE_POWER_CTRL, OUTPUT);
};

void Relay::relayHH()
{
    this->laneA.dWriteHigh();
    this->laneB.dWriteHigh();
    Serial.print("laneA dRead = "); Serial.println(this->laneA.dRead());
    Serial.print("laneB dRead = "); Serial.println(this->laneB.dRead());
};

void Relay::relayLL()
{
    this->laneA.dWriteLow();
    this->laneB.dWriteLow();
    Serial.print("laneA dRead = "); Serial.println(this->laneA.dRead());
    Serial.print("laneB dRead = "); Serial.println(this->laneB.dRead());
};

void Relay::relayHL()
{
    this->laneA.dWriteHigh();
    this->laneB.dWriteLow();
};

void Relay::relayLH()
{
    this->laneA.dWriteLow();
    this->laneB.dWriteHigh();
};

// --------------------------------
// RELAY PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// RELAY PRIVATE METHODS
// --------------------------------