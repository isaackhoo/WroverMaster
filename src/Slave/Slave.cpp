#include "Slave.h"

// ---------------------------
// Slave Constants Declaration
// ---------------------------
const int SlaveConstants::DEFAULT_SERIAL_BAUD_RATE = 115200;

const int SlaveConstants::INST_RACK_ID_LEN = 2;
const int SlaveConstants::INST_COL_ID_LEN = 2;
const int SlaveConstants::INST_BINPOS_LEN = 2;

const int SlaveConstants::MIN_COLUMN = -2;
const int SlaveConstants::MAX_COLUMN = 20;
const int SlaveConstants::MIN_BINSLOT = 1;
const int SlaveConstants::MAX_BINSLOT = 12;

const unsigned long SlaveConstants::SLAVE_PING_INTERVAL = 1000 * 10;        // 10s
const unsigned long SlaveConstants::SLAVE_PING_DROPPED_DURATION = 1000 * 1; // 1s after sending out ping
const unsigned int SlaveConstants::SLAVE_MAX_DROPPED_PINGS = 3;

const String SlaveConstants::HEADER_DELIMITER = ",";
const String SlaveConstants::BODY_DELIMITER = "-";

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
    // y+2-z  - stepAction-stepInstructions
    // z+1    - ETX
    // z+2    - EOT

    String sendString = "";
    if (includeBoundingControlChars)
        sendString += SOH;
    sendString += sendObj.uuid != "" ? sendObj.uuid : String(millis());
    sendString += HEADER_DELIMITER;
    sendString += GET_TWO_DIGIT_STRING(sendObj.messageLength);
    sendString += STX;
    sendString += GET_TWO_DIGIT_STRING(sendObj.action);
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
        this->serialIn += this->ss->readString();
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

    return res;
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

bool Slave::send(Step *step)
{
    SlaveCommsFormat *stepFormat = new SlaveCommsFormat(
        String(step->getStepAction()),
        step->getStepInstructions());
    bool res = this->send(*stepFormat);
    delete stepFormat;
    return res;
};

void Slave::pingSlaveChip()
{
    SlaveCommsFormat *ping = new SlaveCommsFormat(String(SLAVE_PING), "");
    this->pong = false;
    this->pongChecked = false;
    this->lastPingMillis = millis();
    this->send(*ping, false, false);
    delete ping;
};

void Slave::runPing()
{
    if (!this->pingsStarted)
        return;

    unsigned int currentMillis = millis();

    // check for unreplied ping
    if (currentMillis - this->lastPingMillis >= SLAVE_PING_DROPPED_DURATION)
    {
        if (!this->pongChecked)
        {
            this->pongChecked = true;
            if (!this->pong)
            {
                // ping was not replied
                if (this->droppedPings < SLAVE_MAX_DROPPED_PINGS)
                    ++this->droppedPings;
                else
                {
                    logMasterError("Slave dropped " + String(SLAVE_MAX_DROPPED_PINGS) + " pings consecutively. Resetting chip");
                    this->pingsStarted = false;
                }
            }
        }
    }

    // check if it should ping slave again
    if (currentMillis - this->lastPingMillis >= SLAVE_PING_INTERVAL)
        this->pingSlaveChip();
};

void Slave::startPings()
{
    // start pinging slave chip
    this->droppedPings = 0;
    this->pingSlaveChip();
    this->pingsStarted = true;
};

void Slave::updatePingReceived()
{
    this->pong = true;
    this->droppedPings = 0;
};

void Slave::resetSlave()
{
     SlaveCommsFormat *resetCmd = new SlaveCommsFormat(String(SLAVE_RESET), "");
    this->send(*resetCmd, false, false);
    delete resetCmd;
};

SlaveCommsFormat *Slave::interpret(String input)
{
    // receive format
    // x      - uuid - millis
    // x+1    - HEADER_DELIMITER
    // x+2-y  - message length EXCLUDING STX and ETX
    // y+1    - STX
    // y+2-z  - stepAction-stepInstructions
    // z+1    - ETX

    int headerDeliIdx = input.indexOf(HEADER_DELIMITER);
    int stxIdx = input.indexOf(STX, headerDeliIdx + 1);
    int bdyDeliIdx = input.indexOf(BODY_DELIMITER, stxIdx + 1); // can be -1
    int etxIdx = input.indexOf(ETX, stxIdx + 1);

    SlaveCommsFormat *formattedInput = new SlaveCommsFormat();
    formattedInput->uuid = input.substring(0, headerDeliIdx);
    formattedInput->messageLength = input.substring(headerDeliIdx + 1, stxIdx).toInt();
    int endStepActionIdx = bdyDeliIdx > 0 ? bdyDeliIdx : etxIdx;
    formattedInput->action = input.substring(stxIdx + 1, endStepActionIdx);
    if (bdyDeliIdx > 0)
        formattedInput->instructions = input.substring(bdyDeliIdx + 1, etxIdx);

    // validate message
    String msg = input.substring(stxIdx + 1, etxIdx);
    if (formattedInput->messageLength != msg.length())
    {
        logSlaveError("Invalid message - " + msg);
        delete formattedInput;
        return NULL;
    }

    return formattedInput;
};

void Slave::perform(SlaveCommsFormat *formattedInput)
{
    if (formattedInput == NULL)
        return;

    ENUM_SLAVE_ACTIONS action = (ENUM_SLAVE_ACTIONS)formattedInput->action.toInt();

    // echo back message to slave
    if (action != SLAVE_LOGIN && action != SLAVE_PING && action != SLAVE_ECHO)
    {
        SlaveCommsFormat *echoReply = new SlaveCommsFormat(
            formattedInput->uuid,
            String(SLAVE_ECHO),
            "");
        this->send(*echoReply, false, false);
        logMaster(String(formattedInput->messageLength) + " " + formattedInput->action + "-" + formattedInput->instructions);
        delete echoReply;
    }

    switch (action)
    {
    case SLAVE_LOGIN:
    {
        logSlave("Slave chip logged in");
        this->startPings();
        break;
    }
    case SLAVE_ECHO:
    {
        this->echos->verifyEcho(formattedInput->uuid);
        break;
    }
    case LOG:
    {
        logSlave(formattedInput->instructions);
        break;
    }
    case LOGERROR:
    {
        logSlaveError(formattedInput->instructions);
        break;
    }
    case SLAVE_PING:
    {
        this->updatePingReceived();
        break;
    }
    case ENGAGE_ESTOP:
    case DISENGAGE_ESTOP:
    case MOVETO:
    // case READ_BIN_SENSOR:
    case EXTEND_ARM:
    case HOME_ARM:
    case EXTEND_FINGER_PAIR:
    case RETRACT_FINGER_PAIR:
    {
        Step *next = this->taskManager->validateStep(formattedInput->instructions);
        if (next == NULL)
        {
            // task is complete
            logSlave("Task completed");
            // notify server task completion
            this->wcsInstance->notifyTaskCompletion();
        }
        else
        {
            if (next->getStepStatus() == STEP_ERROR)
                logSlaveError(next->getStepErrorDetails());
            else if (next->getStepStatus() == STEP_ACTIVE)
                this->send(next);
            else
                logMasterError("Invalid step status event");
        }
        break;
    }
    case SLAVE_BATTERY:
    {
        // TODO
        break;
    }
    default:
        break;
    }

    // free up memeory taken by formatted input
    delete formattedInput;
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
        this->perform(this->interpret(input));

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

void Slave::updateStatus(String action, String inst)
{
    this->statusInstance->setActionEnum(action);
    this->statusInstance->setInstructions(inst);
    this->statusInstance->setActivityState();
    this->statusInstance->saveStatus();
};

// --------------------------
// Wcs Public Methods
// --------------------------
Slave::Slave()
{
    this->echos = new Echo();
    this->serialIn = "";
    using namespace std::placeholders;
    this->bindedSender = std::bind(static_cast<bool (Slave::*)(String, bool, bool)>(&Slave::send), this, _1, _2, _3);

    this->pingsStarted = false;
    this->lastPingMillis = 0;
    this->pong = false;
    this->droppedPings = 0;

    this->taskManager = NULL;
};

bool Slave::init(HardwareSerial *ss, Status *context)
{
    this->ss = ss;
    this->ss->end();
    this->ss->begin(DEFAULT_SERIAL_BAUD_RATE);
    logMaster("Slave serial started. Baud " + String(DEFAULT_SERIAL_BAUD_RATE));
    this->statusInstance = context;

    // get slave to reset to login
    this->resetSlave();

    this->taskManager = new Task(this->statusInstance);
    return true;
};

void Slave::setWcsInstance(WCS *context)
{
    this->wcsInstance = context;
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

void Slave::onRetrieveBin(String inst)
{
    this->updateStatus(GET_TWO_DIGIT_STRING(RETRIEVEBIN), inst);
    this->taskManager->createRetrievalTask(inst);
    this->send(this->taskManager->begin());
};

void Slave::onStoreBin(String inst)
{
    this->updateStatus(GET_TWO_DIGIT_STRING(STOREBIN), inst);
    this->taskManager->createStorageTask(inst);
    this->send(this->taskManager->begin());
};

void Slave::onMove(String inst)
{
    this->updateStatus(GET_TWO_DIGIT_STRING(MOVE), inst);
    this->taskManager->createMovementTask(inst);
    this->send(this->taskManager->begin());
};

void Slave::onBattery(){
    // TODO
};
