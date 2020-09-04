#include "WCS/WCS.h"

namespace WcsConstants
{
    const unsigned int PING_INTERVAL = 1000 * 10;
    const unsigned int PING_DROPPED_DURATION = 1000 * 2;
    const unsigned int MAX_DROPPED_PINGS = 3;
}; // namespace WcsConstants

// ----------------------------------------------- WCSCOMMS -----------------------------------------------
// -----------------------------------
// WCSCOMMS PUBLIC VARIABLES
// -----------------------------------

// -----------------------------------
// WCSCOMMS PUBLIC METHODS
// -----------------------------------
WcsComms::WcsComms(){};

WcsComms::WcsComms(ENUM_WCS_ACTIONS a, String i)
{
    this->init(a, i);
};

void WcsComms::init(ENUM_WCS_ACTIONS action, String inst)
{
    this->instructions = "";
    this->actionEnum = action;
    this->instructions = inst;
};

String WcsComms::toString(bool includeControlCharacters)
{
    // Send format
    // 1    - STX
    // 2-5  - shuttle ID
    // 6-7  - shuttle Action
    // 8-x  - other information
    // x+1  - ETX

    String str((char *)0);
    str.reserve(128);

    if (includeControlCharacters)
        str += STX;
    str += Status::getId();
    str += GET_TWO_DIGIT_STRING((int)this->actionEnum);
    str += this->instructions;
    if (includeControlCharacters)
        str += ETX;

    return str;
};

String WcsComms::toString()
{
    return toString(true);
};

ENUM_WCS_ACTIONS WcsComms::getActionEnum()
{
    return this->actionEnum;
};

String WcsComms::getInstructions()
{
    return this->instructions;
};

// -----------------------------------
// WCSCOMMS PRIVATE METHODS
// -----------------------------------

// ----------------------------------------------- WCS -----------------------------------------------
// -----------------------------------
// WCS PUBLIC VARIABLES
// -----------------------------------

// -----------------------------------
// WCS PUBLIC METHODS
// -----------------------------------
WCS::WCS(){};

bool WCS::init()
{
    bool res;

    // nullify pointers
    this->slaveInstance = NULL;
    this->completionNotification = NULL;

    // init echos
    this->echoBroker.init(1000 * 5, 3);

    res = TCP::init();
    // connect to server
    res = TCP::ConnectTcpServer();

    // login to server
    this->loginToServer();

    return res;
};

void WCS::setSlaveInstance(Slave *context)
{
    this->slaveInstance = context;
};

void WCS::run()
{
    // check for incoming messages from the server
    if (TCP::TcpRead(ETX))
        this->extractTcpInput();

    // check for pings
    this->runPing();

    // check for echos
    int droppedEchos = this->echoBroker.run();
    if (droppedEchos > 0)
    {
        // resend nodes that have not reached max retries
        while (this->echoBroker.getNextDroppedNode() != NULL)
        {
            // resend the dropped echo
            EchoNode *echo = this->echoBroker.getNextDroppedNode();
            if (echo != NULL)
                this->send(echo->getMessage(), false, true, echo->dropped);

            // remove dropped node
            this->echoBroker.removeCurrentDroppedNode();
        };
    };

    // check if status needs to be updated to server
    if (Status::shouldNotifyStatusChange())
    {
        WcsComms statusUpdate(STATE, GET_TWO_DIGIT_STRING((int)Status::getState()));
        this->send(statusUpdate);

        // turn off toggle
        Status::clearNotification();
    };

    // check if there is task completion to notify server
    if (this->completionNotification != NULL)
    {
        this->send(*this->completionNotification);
        delete this->completionNotification;
        this->completionNotification = NULL;
    }
};

void WCS::loginToServer()
{
    String loginDetails((char *)0);
    loginDetails.reserve(8);

    loginDetails += GET_TWO_DIGIT_STRING(Status::getLevel());
    loginDetails += GET_TWO_DIGIT_STRING(Status::getState());

    WcsComms loginMsg(LOGIN, loginDetails);

    this->send(loginMsg, false, false);
};

void WCS::notifyTaskCompletion(String feedback)
{
    // set shuttle status idle
    Status::setState(IDLE);

    // update that task is complete
    this->completionNotification = new WcsComms((ENUM_WCS_ACTIONS)Status::getActionEnum().toInt(), feedback);

    // remove shuttle task
    Status::clearTask();
};

void WCS::notifyTaskCompletion()
{
    this->notifyTaskCompletion("");
};

void WCS::notifyTaskFailure(String err)
{
    // set shuttle status error
    Status::setState(ENUM_SHUTTLE_STATE::SHUTTLE_ERROR);

    // update that task has error
    this->completionNotification = new WcsComms(ENUM_WCS_ACTIONS::ERROR, err);

    // remove shuttle task
    Status::clearTask();
};

void WCS::updateBatteryLevel(String battery)
{
    WcsComms batteryUpdate(ENUM_WCS_ACTIONS::BATTERY, battery);
    this->send(batteryUpdate);
};

// -----------------------------------
// WCS PRIVATE VARIABLES
// -----------------------------------

// -----------------------------------
// WCS PRIVATE METHODS
// -----------------------------------
bool WCS::send(String toSend, bool shouldLog, bool awaitEcho, unsigned int retries)
{
    bool res = TCP::TcpWrite(toSend);

    // check if msg should be logged
    if (shouldLog)
    {
        String sendLog((char *)0);
        sendLog.reserve(128);

        if (!res)
            sendLog += F("Failed to send to server");
        else
        {
            sendLog += "<< ";
            sendLog += toSend;
        }
        Logger::logWcs(sendLog);
    }

    // check if should await msg for echo
    if (awaitEcho)
    {
        // remove control characters from send string
        String echoUuid((char *)0);
        echoUuid.reserve(128);

        echoUuid += toSend.substring(1, toSend.length() - 1);
        // push to echo list
        this->echoBroker.push(echoUuid, toSend, retries);
    }

    return res;
};

bool WCS::send(String s, bool sl, bool ae)
{
    return this->send(s, sl, ae, 0);
};

bool WCS::send(WcsComms c, bool sl, bool ae)
{
    return this->send(c.toString(), sl, ae);
};

bool WCS::send(WcsComms c, bool sl)
{
    return this->send(c, sl, true);
};

bool WCS::send(WcsComms c)
{
    return this->send(c, true);
};

void WCS::pingServer()
{
    WcsComms pingServer(PING, "");
    this->pong = false;
    this->pongChecked = false;
    this->lastPingMillis = millis();
    this->send(pingServer, false, false);
};

void WCS::runPing()
{
    unsigned int currentMillis = millis();

    // check for unreplied ping
    if (currentMillis - this->lastPingMillis >= PING_DROPPED_DURATION)
    {
        if (!this->pongChecked)
        {
            this->pongChecked = true;
            if (!this->pong)
            {
                // ping was not replied
                if (this->droppedPings < MAX_DROPPED_PINGS)
                    ++this->droppedPings;
                else
                {
                    Logger::logWcsError(F("Max pings dropped. Resetting chip"));
                    ESPWrover::reset();
                }
            }
        }
    }

    // check if its time to ping server again
    if (currentMillis - this->lastPingMillis >= PING_INTERVAL)
        this->pingServer();
};

void WCS::startPings()
{
    this->pingServer();
};

void WCS::updatePingReceived()
{
    this->pong = true;
    this->droppedPings = 0;
};

void WCS::extractTcpInput()
{
    // input details are stored within TCP
    int stxIdx = TCP::tcpIn.indexOf(STX);
    int etxIdx = TCP::tcpIn.indexOf(ETX);

    while (etxIdx >= 0 && etxIdx > stxIdx)
    {
        // extract input, excluding STX and ETX
        String input((char *)0);
        input.reserve(64);
        input = TCP::tcpIn.substring(stxIdx + 1, etxIdx);
        if (etxIdx + 1 == TCP::tcpIn.length())
            TCP::tcpIn = "";
        else
            TCP::tcpIn = TCP::tcpIn.substring(etxIdx + 1);

        // interpret & run input
        this->perform(this->interpret(input));

        // check for more inputs
        if (TCP::tcpIn.length() > 0)
        {
            stxIdx = TCP::tcpIn.indexOf(STX);
            etxIdx = TCP::tcpIn.indexOf(ETX);
        }
        else
        {
            stxIdx = -1;
            etxIdx = -1;
        }
    }
};

WcsComms WCS::interpret(String input)
{
    // interpret input without STX and ETX
    // expected input:
    // iiii = id
    // aa = wcsActionEnum
    // x* = instructions

    String receivedId((char *)0);
    receivedId.reserve(5);
    String action((char *)0);
    action.reserve(3);
    String instructions((char *)0);
    instructions.reserve(32);

    int fromIdx = 0;

    receivedId = input.substring(fromIdx, fromIdx + StatusConstants::DEFAULT_ID_LENGTH);
    fromIdx += (StatusConstants::DEFAULT_ID_LENGTH);
    action = input.substring(fromIdx, fromIdx + Helper::DEFAULT_ENUM_VALUE_LENGTH);
    fromIdx += (Helper::DEFAULT_ENUM_VALUE_LENGTH);
    instructions = input.substring(fromIdx);

    WcsComms interpretedInput;

    // validate input
    if (receivedId != Status::getId())
    {
        interpretedInput.init(ERROR, F("Mismatched ids"));
    }
    else
    {
        interpretedInput.init((ENUM_WCS_ACTIONS)action.toInt(), instructions);
    }

    return interpretedInput;
};

void WCS::perform(WcsComms input)
{
    // update status
    if (input.getActionEnum() == RETRIEVEBIN || input.getActionEnum() == STOREBIN || input.getActionEnum() == MOVE || input.getActionEnum() == BUFFER_TRANSFER)
    {
        Status::setTask(input.getActionEnum(), input.getInstructions());
    };

    // echo back input
    if (input.getActionEnum() != PING && input.getActionEnum() != LOGIN && input.getActionEnum() != ECHO)
    {
        WcsComms echo(ECHO, input.toString(false));
        this->send(echo, true, false);

        // log received input
        String received((char *)0);
        received.reserve(128);
        received += ">> [";
        received += EnumWcsActionString[(int)input.getActionEnum()];
        received += "] ";
        received += input.getInstructions();
        Logger::logWcs(received);
    };

    // determine action to perform
    switch (input.getActionEnum())
    {
    case LOGIN:
    {
        log(F("Logged in to server"));

        // start pings to server
        this->startPings();
        break;
    }
    case LOGOUT:
    {
        // do nothing
        break;
    }
    case PING:
    {
        this->updatePingReceived();
        break;
    }
    case RETRIEVEBIN:
    {
        // hand over control to slave handler
        if (this->slaveInstance != NULL)
            this->slaveInstance->onRetrieveBin(input.getInstructions());
        break;
    }
    case STOREBIN:
    {
        // hand over control to slave handler
        if (this->slaveInstance != NULL)
            this->slaveInstance->onStoreBin(input.getInstructions());
        break;
    }
    case MOVE:
    {
        // hand over control to slave handler
        if (this->slaveInstance != NULL)
            this->slaveInstance->onMove(input.getInstructions());
        break;
    }
    // case BATTERY:
    // {
    //     // hand over control to slave handler
    //     if (this->slaveInstance != NULL)
    //         this->slaveInstance->onBattery();
    //     break;
    // }
    case STATE:
    {
        // do nothing for now. not in use
        break;
    }
    case LEVEL:
    {
        Status::setLevel(input.getInstructions());
        Status::saveStatus();
        break;
    }
    case SET:
    {
        // interpret manual set instructions

        ENUM_MANUAL_SET_TYPES manualAction = (ENUM_MANUAL_SET_TYPES)input.getInstructions().substring(0, DEFAULT_ENUM_VALUE_LENGTH).toInt();
        String manualInstructions((char *)0);
        manualInstructions.reserve(16);
        manualInstructions = input.getInstructions().substring(DEFAULT_ENUM_VALUE_LENGTH);

        String manualSetLog((char *)0);
        manualSetLog.reserve(64);

        // determine manual switch action to perform
        switch (manualAction)
        {
        case MANUAL_SET_DEFAULT:
        {
            // not in use
            break;
        }
        case MANUAL_SET_ID:
        {
            Status::setId(manualInstructions);
            Status::saveStatus();
            manualSetLog += F("[Manual] Id set to ");
            manualSetLog += Status::getId();
            break;
        }
        case MANUAL_SET_LEVEL:
        {
            Status::setLevel(manualInstructions);
            Status::saveStatus();
            manualSetLog += F("[Manual] Level set to ");
            manualSetLog += Status::getLevel();
            break;
        }
        case MANUAL_SET_STATE:
        {
            Status::setState((StatusConstants::ENUM_SHUTTLE_STATE)manualInstructions.toInt());
            Status::saveStatus();
            manualSetLog += F("[Manual] State set to ");
            manualSetLog += Status::getStateString();
            break;
        }
        case MANUAL_SET_SLOTHOLE:
        {
            Status::setSlothole(manualInstructions);
            Status::saveStatus();
            manualSetLog += F("[Manual] Slothole set to ");
            manualSetLog += manualInstructions;

            // update slave chip that slothole has been changed
            if (this->slaveInstance != NULL)
                this->slaveInstance->updateSlothole();
            break;
        }
        case MANUAL_SET_ECHO:
        {
            this->echoBroker.reset();
            manualSetLog += F("[Manual] Echos reset");
            break;
        }
        default:
        {
            break;
        }
        }

        log(manualSetLog);
        break;
    }
    case BUFFER_TRANSFER:
    {
        // hand over control to slave handler
        if (this->slaveInstance != NULL)
            this->slaveInstance->onBufferTransfer(input.getInstructions());
        break;
    }
    case RECEIVE:
    {
        // hand over control to slave handler
        if (this->slaveInstance != NULL)
            this->slaveInstance->onReceive(input.getInstructions());
        break;
    }
    case RELEASE:
    {
        // hand over control to slave handler
        if (this->slaveInstance != NULL)
            this->slaveInstance->onRelease(input.getInstructions());
        break;
    }
    case ECHO:
    {
        this->echoBroker.verify(input.getInstructions());
        break;
    }

    case ERROR:
    {
        // do nothing. not in use
        break;
    }
    default:
        break;
    }
};