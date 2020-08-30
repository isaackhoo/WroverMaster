#include "Slave/Slave.h"

namespace SlaveConstants
{
    const int DEFAULT_SERIAL_BAUD_RATE = 115200;

    // pings
    const unsigned long SLAVE_PING_INTERVAL = 1000 * 35; // allows up to 3 retries

    // serial communications
    const String HEADER_DELIMITER(F(","));
    const String BODY_DELIMITER(F("_"));
}; // namespace SlaveConstants

// ----------------------------------------- SLAVECOMMS -----------------------------------------
// -------------------------------
// SLAVECOMMS PUBLIC VARIABLES
// -------------------------------
SlaveComms::SlaveComms(){};

SlaveComms::SlaveComms(String u, ENUM_SLAVE_ACTIONS a, String i)
{
    this->init(u, a, i);
};

SlaveComms::SlaveComms(ENUM_SLAVE_ACTIONS a, String i) : SlaveComms(GEN_UUID(), a, i){};

void SlaveComms::init(String uuid, ENUM_SLAVE_ACTIONS action, String inst)
{
    this->uuid = uuid;
    this->action = action;
    this->instructions = inst;
    this->msgLength = 0;

    // calculate message length
    this->msgLength += DEFAULT_ENUM_VALUE_LENGTH;
    if (this->instructions.length() > 0)
    {
        this->msgLength += BODY_DELIMITER.length();
        this->msgLength += this->instructions.length();
    }
};

void SlaveComms::init(ENUM_SLAVE_ACTIONS a, String i)
{
    this->init(GEN_UUID(), a, i);
};

String SlaveComms::toString(bool includeBoundingControlChars)
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

    String str((char *)0);
    str.reserve(128);

    if (includeBoundingControlChars)
        str += SOH;
    str += this->getUuid();
    str += HEADER_DELIMITER;
    str += GET_TWO_DIGIT_STRING(this->getMsgLength());
    str += STX;
    str += GET_TWO_DIGIT_STRING((int)this->getAction());
    if (this->instructions.length() > 0)
    {
        str += BODY_DELIMITER;
        str += this->getInstructions();
    }
    str += ETX;
    if (includeBoundingControlChars)
        str += EOT;

    return str;
};

String SlaveComms::toString()
{
    return this->toString(true);
};

String SlaveComms::getUuid()
{
    return this->uuid;
};

ENUM_SLAVE_ACTIONS SlaveComms::getAction()
{
    return this->action;
};

String SlaveComms::getInstructions()
{
    return this->instructions;
};

int SlaveComms::getMsgLength()
{
    return this->msgLength;
};

// -------------------------------
// SLAVECOMMS PUBLIC METHODS
// -------------------------------

// -------------------------------
// SLAVECOMMS PRIVATE VARIABLES
// -------------------------------

// -------------------------------
// SLAVECOMMS PRIVATE METHODS
// -------------------------------

// ----------------------------------------- SLAVE -----------------------------------------
// -------------------------------
// SLAVE PUBLIC VARIABLES
// -------------------------------

// -------------------------------
// SLAVE PUBLIC METHODS
// -------------------------------
Slave::Slave(){};

bool Slave::init(HardwareSerial *ss)
{
    bool res = true;
    // Nullify pointers
    this->wcsInstance = NULL;

    // reserve string space
    this->serialIn.reserve(1032);

    // default pings
    this->isSerialConnected = false;
    this->lastSerialPingMillis = 0;
    this->lastSlaveResetMillis = 0;

    // initialize task manager
    this->taskManager.init();

    // initialize echos
    this->echoBroker.init(1000 * 5, 3);
    // set serial and begin
    this->ss = ss;
    this->ss->end();
    this->ss->begin(DEFAULT_SERIAL_BAUD_RATE);

    // battery
    this->lastBatteryReqMillis = 0;

    return res;
};

void Slave::setWcsInstance(WCS *context)
{
    this->wcsInstance = context;
};

void Slave::run()
{
    // // if slave chip is not logged in, try to get it to reset
    // if (!this->isSerialConnected && millis() - this->lastSlaveResetMillis >= SLAVE_CHIP_RESET_INTERVAL)
    // {
    //     this->lastSlaveResetMillis = millis();
    //     SlaveComms resetInst(SLAVE_RESET, "");
    //     this->send(resetInst, true, false);
    // }

    // check for slave chip message
    if (this->serialRead())
        this->extractSerialInput();

    // check for pings
    this->runPing();

    // check for echos
    int droppedEchos = this->echoBroker.run();
    if (droppedEchos > 0)
    {
        // resend nodes that have not reached max retries
        while (this->echoBroker.getNextDroppedNode() != NULL)
        {
            // resend dropped echo
            EchoNode *echo = this->echoBroker.getNextDroppedNode();
            if (echo != NULL)
                this->send(echo->getMessage(), false, true, echo->dropped);

            // remove dropped node
            this->echoBroker.removeCurrentDroppedNode();
        }
    }

    // check for battery level
    if (millis() - this->lastBatteryReqMillis >= BATTERY_REQ_INTERVAL)
    {
        this->lastBatteryReqMillis = millis();
        SlaveComms batteryReq = SlaveComms(ENUM_SLAVE_ACTIONS::SLAVE_BATTERY, "");
        this->send(batteryReq, false);
    }
};

void Slave::updateSlothole()
{
    SlaveComms updateSlothole = SlaveComms(UPDATE_SLOTHOLE, Status::getSlothole());
    this->send(updateSlothole);
};

void Slave::onRetrieveBin(String inst)
{
    this->taskManager.createRetrievalTask(inst);
    this->startTask();
};

void Slave::onStoreBin(String inst)
{
    this->taskManager.createStorageTask(inst);
    this->startTask();
};

void Slave::onMove(String inst)
{
    this->taskManager.createMovementTask(inst);
    this->startTask();
};

void Slave::onBufferTransfer(String from)
{
    ENUM_EXTENSION_DIRECTION fromDirection = (ENUM_EXTENSION_DIRECTION)from.toInt();
    if (fromDirection != LEFT && fromDirection != RIGHT)
        return;

    this->taskManager.createBufferTransferTask(from);
    this->startTask();
};

// -------------------------------
// SLAVE PRIVATE VARIABLES
// -------------------------------

// -------------------------------
// SLAVE PRIVATE METHODS
// -------------------------------
bool Slave::send(String toSend, bool shouldLog, bool awaitEcho, unsigned int retries)
{
    // if (!this->isSerialConnected)
    // {
    //     logError(F("Slavechip is not logged in"));
    //     return false;
    // }

    bool res = this->ss->print(toSend);

    // check if should log
    if (shouldLog)
    {
        String sendLog((char *)0);
        sendLog.reserve(128);

        if (!res)
            sendLog += F("Failed to send to slave chip");
        else
        {
            sendLog += toSend;
            sendLog += F(" >>");
        }
        Logger::logSlave(sendLog);
    }

    // check if should await msg echo
    if (awaitEcho)
    {
        // retrieve uuid from send string
        String uuid((char *)0);
        uuid.reserve(64);
        int headerDeliIdx = toSend.indexOf(HEADER_DELIMITER);
        uuid += toSend.substring(1, headerDeliIdx); // ignore SOH char

        // push to echo list
        this->echoBroker.push(uuid, toSend, retries);
    }

    return res;
};

bool Slave::send(String s, bool sl, bool ae)
{
    return this->send(s, sl, ae, 0);
};

bool Slave::send(SlaveComms c, bool sl, bool ae)
{
    return this->send(c.toString(), sl, ae);
};

bool Slave::send(SlaveComms c, bool sl)
{
    return this->send(c, sl, true);
};

bool Slave::send(SlaveComms c)
{
    return this->send(c, true);
};

bool Slave::send(Step *step)
{
    if (step == NULL)
        return false;

    SlaveComms stepInstructions(step->getStepAction(), String(step->getStepTarget()));
    return this->send(stepInstructions);
};

bool Slave::serialRead()
{
    if (this->ss != NULL && this->ss->available() > 0)
    {
        serialIn += this->ss->readStringUntil(EOT);
        serialIn += EOT;
        return true;
    };
    return false;
};

void Slave::extractSerialInput()
{
    // input details are stored within TCP
    int sohIdx = this->serialIn.indexOf(SOH);
    int eotIdx = this->serialIn.indexOf(EOT);

    while (eotIdx >= 0 && eotIdx > sohIdx)
    {
        // extract input, excluding STX and ETX
        String input((char *)0);
        input.reserve(64);
        input = this->serialIn.substring(sohIdx + 1, eotIdx);
        if (eotIdx + 1 == this->serialIn.length())
            this->serialIn = "";
        else
            this->serialIn = this->serialIn.substring(eotIdx + 1);

        // interpret & run input
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
};

SlaveComms Slave::interpret(String input)
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
    int etxIdx = input.indexOf(ETX, stxIdx + 1);

    String uuid((char *)0);
    uuid.reserve(64);
    int msgLength;
    String message((char *)0);
    message.reserve(128);

    uuid = input.substring(0, headerDeliIdx);
    msgLength = input.substring(headerDeliIdx + 1, stxIdx).toInt();
    message = input.substring(stxIdx + 1, etxIdx);

    SlaveComms formattedInput;

    // validate message integrity
    if (msgLength != message.length())
    {
        formattedInput.init(SLAVE_ERROR, "Mismatch message length " + message);
    }
    else
    {
        int bdyDeliIdx = message.indexOf(BODY_DELIMITER); // can be -1
        String actionStr((char *)0);
        actionStr.reserve(3);
        actionStr += bdyDeliIdx > 0 ? message.substring(0, bdyDeliIdx) : message;
        ENUM_SLAVE_ACTIONS action = (ENUM_SLAVE_ACTIONS)actionStr.toInt();
        String inst((char *)0);
        inst.reserve(64);
        inst += bdyDeliIdx > 0 ? message.substring(bdyDeliIdx + 1) : "";

        formattedInput.init(uuid, action, inst);
    }

    return formattedInput;
};

void Slave::perform(SlaveComms input)
{
    // echo back message to slave
    if (input.getAction() != SLAVE_LOGIN && input.getAction() != SLAVE_PING && input.getAction() != SLAVE_ECHO)
    {
        SlaveComms echo(input.getUuid(), SLAVE_ECHO, "");
        this->send(echo, false, false);

        if (input.getAction() != LOG && input.getAction() != LOGERROR && input.getAction() != SLAVE_ERROR)
        {
            // log input
            String received((char *)0);
            received.reserve(128);
            received += input.toString();
            received += " <<";
            logSlave(received);
        }
    };

    // determine slave action to perform
    switch (input.getAction())
    {
    case SLAVE_LOGIN:
    {
        this->isSerialConnected = true;
        logSlave(F("Slave chip logged in"));

        // reset ping millis
        this->lastSerialPingMillis = millis();

        // provide slave with current slothole information
        String currentSlothole = GET_TWO_DIGIT_STRING(Status::getSlothole());

        // reply to slave chip that login is successful
        SlaveComms loginSuccess(SLAVE_LOGIN, currentSlothole);
        this->send(loginSuccess, true, false);

        break;
    }
    case SLAVE_ECHO:
    {
        this->echoBroker.verify(input.getUuid());
        break;
    }
    case LOG:
    {
        logSlave(input.getInstructions());
        break;
    }
    case LOGERROR:
    {
        logSlaveError(input.getInstructions());
        break;
    }
    case SLAVE_PING:
    {
        if (!this->isSerialConnected)
            break;

        this->lastSerialPingMillis = millis();
        // reply ping
        SlaveComms pingReply(SLAVE_PING, "");
        this->send(pingReply, false, false);
        break;
    }
    case ENGAGE_ESTOP:
    case DISENGAGE_ESTOP:
    case MOVETO:
    case READ_BIN_SENSOR:
    case EXTEND_ARM:
    case HOME_ARM:
    case EXTEND_FINGER_PAIR:
    case RETRACT_FINGER_PAIR:
    {
        // let task manager handle
        Step *next = this->taskManager.validateCurrentStep(input.getInstructions());
        if (next == NULL)
        {
            // overall task is complete
            logSlave(F("Task completed"));
            // notify server task completion
            if (this->wcsInstance != NULL)
            {
                Logger::log("status instructions " + Status::getInstructions());
                this->wcsInstance->notifyTaskCompletion(Status::getInstructions());
            }
        }
        // has more task to perform
        else
        {
            switch (next->getStepStatus())
            {
            case STEP_ERROR:
            {
                Logger::log("entered step error case");
                logSlaveError(next->getStepErrorDetails());
                break;
            }
            case STEP_ACTIVE:
            {
                this->send(next);
                break;
            }
            default:
            {
                logError(F("Invalid step status encountered"));
                break;
            }
            }
        }

        // update current slothole
        if (input.getAction() == MOVETO)
        {
            if (next != NULL && next->getStepAction() == MOVETO)
            {
                // failed to complete movement
                break;
            }
            else
            {
                // successfully completed movement step
                // update status with current slothole
                Status::setSlothole(input.getInstructions());
                Status::saveStatus();
            }
        }
        break;
    }
    case SLAVE_BATTERY:
    {
        // received battery percentage feedback
        double batteryPerc = input.getInstructions().toDouble() / 10.0;
        Logger::log("Battery %: " + String(batteryPerc));
        // update master battery
        if (this->wcsInstance != NULL)
            this->wcsInstance->updateBatteryLevel(input.getInstructions());
        break;
    }
    case SLAVE_ERROR:
    {
        logError(input.getInstructions());
        break;
    }
    default:
        break;
    }
};

void Slave::runPing()
{
    if (!this->isSerialConnected)
        return;

    unsigned int currentMillis = millis();

    // check if pings have stopped
    if (currentMillis - this->lastSerialPingMillis >= SLAVE_PING_INTERVAL)
    {
        // slave has stopped pining
        logSlaveError(F("Pings from slave chip stopped"));
        this->isSerialConnected = false;
    };
};

void Slave::startTask()
{
    this->send(this->taskManager.begin());
};