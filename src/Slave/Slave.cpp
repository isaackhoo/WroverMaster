#include "./Slave.h"

// --------------------------
// Wcs Public Variables
// --------------------------
Slave *slave = new Slave();

// --------------------------
// Wcs Private Methods
// --------------------------
String Slave::createSendString(SlaveCommsFormat sendObj, bool includeBoundingControlChars)
{
    // send format
    // 1      - SOH
    // x      - uuid - millis
    // x+1    - HEADER_DELIMITER
    // x+2-y  - message length EXCLUDING STX and ETX
    // y+1    - STX
    // y+2-z  - componentId-componentAction-instructions
    // z+1    - ETX
    // z+2    - EOT

    int msgLength =
        sendObj.componentId.length() + BODY_DELIMITER.length() + sendObj.componentAction.length() + BODY_DELIMITER.length() + sendObj.instructions.length();

    String sendString = "";
    if (includeBoundingControlChars)
        sendString += SOH;
    sendString += String(millis());
    sendString += HEADER_DELIMITER;
    sendString += GET_TWO_DIGIT_STRING(msgLength);
    sendString += STX;
    sendString += sendObj.componentId;
    sendString += BODY_DELIMITER;
    sendString += GET_TWO_DIGIT_STRING(sendObj.componentAction);
    if (sendObj.instructions != "")
    {
        sendString += BODY_DELIMITER;
        sendString += sendObj.instructions;
    }
    sendString += ETX;
    if (includeBoundingControlChars)
        sendString += EOT;

    return sendString;
};
String Slave::createSendString(SlaveCommsFormat sendObj)
{
    return this->createSendString(sendObj, true);
};

bool Slave::serialRead()
{
    if (this->ss->available() > 0)
    {
        this->serialIn = this->ss->readString();
        return true;
    }
    return false;
}

bool Slave::send(String msg, bool shouldLog, bool awaitEcho)
{
    bool res = this->ss->print(msg);

    // check if msg should be logged
    if (shouldLog)
    {
        String sendLog = "";
        if (!res)
            sendLog = "Failed to send to slave chip";
        else
        {
            sendLog = msg;
            sendLog += " >>";
        }
        logSlave(sendLog);
    }

    // check if message should await echo
    if (awaitEcho)
    {
        // first string in header stores msg uuid
        int sohIdx = msg.indexOf(SOH);
        int headerDelimiterIdx = msg.indexOf(HEADER_DELIMITER);
        String echoUuid = msg.substring(sohIdx + 1, headerDelimiterIdx);
        // push to echos list
        this->echos->pushEcho(
            echoUuid,
            millis(),
            msg.length(),
            msg);
    }
};

bool Slave::send(SlaveCommsFormat sendObj, bool shouldLog, bool awaitEcho)
{
    return this->send(createSendString(sendObj), shouldLog, awaitEcho);
};

bool Slave::send(SlaveCommsFormat sendObj, bool shouldLog)
{
    return this->send(sendObj, shouldLog, true);
};

bool Slave::send(SlaveCommsFormat sendObj)
{
    return this->send(sendObj, true, true);
};

void Slave::pingSlaveChip()
{
    SlaveCommsFormat ping;
    // TODO
};

void Slave::runPing()
{

};

void Slave::startPings()
{

};

void Slave::handleSerialInput()
{
    int sohIdx = this->serialIn.indexOf(SOH);
    int eotIdx = this->serialIn.indexOf(EOT);

    while (eotIdx >= 0 && eotIdx > sohIdx)
    {
        // extract input, excluding SOH and EOT
        String input = this->serialIn.substring(sohIdx + 1, eotIdx);
        if (eotIdx + 1 == this->serialIn.length())
            this->serialIn = "";
        else
            this->serialIn = this->serialIn.substring(eotIdx + 1);

        // interpret and run input
        // TODO

        // check for more inputs
        if (this->serialIn.length() > 0)
        {
            sohIdx = this->serialIn.indexOf(SOH);
            eotIdx = this->serialIn.indexOf(EOT);
        }
        else
        {
            sohIdx = -1;
            eotIdx = -1;
        }
    }
}

// --------------------------
// Wcs Public Methods
// --------------------------
Slave::Slave()
{
    this->echos = new Echo();
    this->serialIn = "";
    using namespace std::placeholders;
    this->bindedSender = std::bind(static_cast<bool (Slave::*)(String, bool, bool)>(&Slave::send), this, _1, _2, _3);

    this->lastPingMillis = 0;
    this->pong = false;
    this->droppedPings = 0;
};

bool Slave::init(HardwareSerial *serialPort)
{
    this->ss = serialPort;
    this->ss->end();
    this->ss->begin(DEFAULT_SERIAL_BAUD_RATE);
    logMaster("Slave serial started. Baud " + String(DEFAULT_SERIAL_BAUD_RATE));
};

void Slave::run()
{
    // check for slave chip message
    if (this->serialRead())
        this->handleSerialInput();

    // check pings
    this->runPing();

    // check echos
    this->echos->run(logMasterError, this->bindedSender);
}