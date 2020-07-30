#include "./WCS.h"

// --------------------------
// Wcs Private Variables
// --------------------------
const unsigned long PING_INTERVAL = 1000 * 10;        // 10s
const unsigned long PING_DROPPED_DURATION = 1000 * 1; // 1s after sending out ping
const unsigned int MAX_DROPPED_PINGS = 3;

const unsigned long ECHO_TIMEOUT_DURATION = 1000 * 5; // 5s

// --------------------------
// Wcs Public Variables
// --------------------------
WCS *wcs = new WCS();

// --------------------------
// Wcs Private Methods
// --------------------------
String WCS::createSendString(WcsCommsFormat sendObj, bool includeBoundingControlChars)
{
    // Send format
    // 1    - STX
    // 2-5  - shuttle ID
    // 6-7  - shuttle Action
    // 8-x  - other information
    // x+1  - ETX

    String sendString = "";
    if (includeBoundingControlChars)
        sendString += STX;
    sendString += status->getId();
    sendString += GET_TWO_DIGIT_STRING(sendObj.actionEnum);
    sendString += sendObj.instructions;
    if (includeBoundingControlChars)
        sendString += ETX;

    return sendString;
};

String WCS::createSendString(WcsCommsFormat sendObj)
{
    return this->createSendString(sendObj, true);
};

bool WCS::send(String msg, bool shouldLog, bool awaitEcho)
{

    bool res = TcpWrite(msg.c_str());

    // check if msg should be logged
    if (shouldLog)
    {
        String sendLog = "";
        if (!res)
            sendLog = "Failed to send to server";
        else
        {
            sendLog = "<< ";
            sendLog += msg;
        }
        logWCS(sendLog);
    }

    // check if msg should await echo
    if (awaitEcho)
    {
        // remove stx and etx from msg to use as echo uuid
        String echoUuid = msg.substring(1, msg.length() - 2);
        // push to echo list
        this->echos->pushEcho(
            echoUuid,
            millis(),
            msg.length(),
            msg);
    }

    return res;
};

bool WCS::send(WcsCommsFormat sendObj, bool shouldLog, bool awaitEcho)
{
    return this->send(createSendString(sendObj), shouldLog, awaitEcho);
};
bool WCS::send(WcsCommsFormat sendObj, bool shouldLog)
{
    return this->send(sendObj, shouldLog, true);
};
bool WCS::send(WcsCommsFormat sendObj)
{
    return this->send(sendObj, true, true);
};

void WCS::pingServer()
{
    WcsCommsFormat ping;
    ping.actionEnum = PING;
    ping.instructions = "";
    this->pong = false;
    this->pongChecked = false;
    this->lastPingMillis = millis();
    this->send(ping, false, false);
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
                    logMasterError("Server dropped " + String(MAX_DROPPED_PINGS) + " pings consecutively. Resetting chip");
                    resetChip();
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
    // start pinging server
    this->pingServer();
};

void WCS::updatePingReceived()
{
    this->pong = true;
    this->droppedPings = 0;
};

WcsCommsFormat *WCS::interpret(String input)
{
    // expected input:
    // iiii = id
    // aa = wcsActionEnum
    // x* = instructions

    int fromIdx = 0;

    String id = input.substring(fromIdx, fromIdx + DEFAULT_ID_LENGTH); // 0-3
    fromIdx += (DEFAULT_ID_LENGTH);
    String actionEnum = input.substring(fromIdx, fromIdx + DEFAULT_ENUM_VALUE_LENGTH); // 4-5
    fromIdx += (DEFAULT_ENUM_VALUE_LENGTH);
    String inst = input.substring(fromIdx);

    WcsCommsFormat *formattedInput = new WcsCommsFormat();
    formattedInput->actionEnum = actionEnum;
    formattedInput->instructions = inst;

    return formattedInput;
};

void WCS::perform(WcsCommsFormat *formattedInput)
{
    ENUM_WCS_ACTIONS action = (ENUM_WCS_ACTIONS)formattedInput->actionEnum.toInt();

    // update status
    if (action == RETRIEVEBIN || action == STOREBIN || action == MOVE)
    {
        status->setActionEnum(formattedInput->actionEnum);
        status->setInstructions(formattedInput->instructions);
        status->saveStatus();
    }

    // Echo back input
    if (action != PING && action != LOGIN && action != ECHO)
    {
        WcsCommsFormat echoReply;
        echoReply.actionEnum = ECHO;
        echoReply.instructions = this->createSendString(*formattedInput, false);
        this->send(echoReply, true, false);
    }

    // determine action to perform
    String recLog = ">> ";
    bool shouldLogReceived = true;

    String additionalLogs = "";
    String errorLogs = "";

    switch (action)
    {
    case LOGIN:
    {
        recLog += "LOGIN";
        // update ID
        status->setId(formattedInput->id);
        status->saveStatus();

        // start pings to server
        this->startPings();
        break;
    }
    case LOGOUT:
    {
        recLog += "LOGOUT";
        // do nothing
        break;
    }
    case PING:
    {
        shouldLogReceived = false;
        // just reply ping - change to shuttle triggers ping instead
        // this->send(*formattedInput, false, false);
        this->updatePingReceived();
        break;
    }
    case RETRIEVEBIN:
    {
        recLog += "RETRIEVEBIN";
        // hand over control to slave handler
        slave->onRetrieveBin(formattedInput->instructions);
        break;
    }
    case STOREBIN:
    {
        recLog += "STOREBIN";
        // hand over control to slave handler
        slave->onStoreBin(formattedInput->instructions);
        break;
    }
    case MOVE:
    {
        recLog += "MOVE";
        // hand over control to slave handler
        slave->onMove(formattedInput->instructions); 
        break;
    }
    case BATTERY:
    {
        recLog += "BATTERY";
        // hand over control to slave handler
        slave->onBattery();
        break;
    }
    case STATE:
    {
        recLog += "STATE";
        // do nothing for now. not in use
        break;
    }
    case LEVEL:
    {
        recLog += "LEVEL";
        status->setLevel(formattedInput->instructions);
        status->saveStatus();
        break;
    }
    case SET:
    {
        recLog += "MANUAL_SET";
        // interpret manual set instructions
        String manualSetAction = formattedInput->instructions.substring(0, DEFAULT_ENUM_VALUE_LENGTH);
        String manualInstructions = formattedInput->instructions.substring(DEFAULT_ENUM_VALUE_LENGTH + 1);
        ENUM_MANUAL_SET_TYPES manualAction = (ENUM_MANUAL_SET_TYPES)manualSetAction.toInt();
        switch (manualAction)
        {
        case MANUAL_SET_DEFAULT:
        {
            // set status to chip defaults, but do not save
            status->setDefault();
            break;
        }
        case MANUAL_SET_ID:
        {
            status->setId(manualInstructions);
            status->saveStatus();
            additionalLogs = "[Manual] ID updated to " + manualInstructions;
            break;
        }
        case MANUAL_SET_LEVEL:
        {
            status->setLevel(manualInstructions);
            status->saveStatus();
            additionalLogs = "[Manual] Level updated to " + manualInstructions;
            break;
        }
        case MANUAL_SET_STATE:
        {
            status->setState((ENUM_SHUTTLE_STATE)manualInstructions.toInt());
            status->saveStatus();
            additionalLogs = String("[Manual] Shuttle State updated to ") + String(SHUTTLE_STATE_STRING[manualInstructions.toInt()]);
            break;
        }
        case MANUAL_SET_ECHO:
        {
            this->echos->reset();
            additionalLogs = "[Manual] Echos reset";
            break;
        }
        default:
        {
            recLog += "Failed to determine action";
            break;
        }
        }
        break;
    }
    case ECHO:
    {
        shouldLogReceived = false;
        if (!this->echos->verifyEcho(formattedInput->instructions))
        {
            // failed to validate echo
            errorLogs = "Echo not found in list. " + String(this->echos->getTotalEchos()) + " echos exist in list.";
        }
        break;
    }
    case ERROR:
    {
        recLog += "ERROR";
        // do nothing. not in use
        break;
    }
    default:
        break;
    }

    // log received
    if (shouldLogReceived)
        logWCS(recLog);
    if (additionalLogs != "")
        logMaster(additionalLogs);
    if (errorLogs != "")
        logMasterError(errorLogs);

    // remove input upon completion
    delete formattedInput;
};

void WCS::handleTcpInput()
{
    // input details are stored within TCP
    int stxIdx = tcpIn.indexOf(STX);
    int etxIdx = tcpIn.indexOf(ETX);

    while (etxIdx >= 0 && etxIdx > stxIdx)
    {
        // extract input, excluding STX and ETX
        String input = tcpIn.substring(stxIdx + 1, etxIdx);
        if (etxIdx + 1 == tcpIn.length())
            tcpIn = "";
        else
            tcpIn = tcpIn.substring(etxIdx + 1);

        // interpret & run input
        this->perform(this->interpret(input));

        // check for more inputs
        if (tcpIn.length() > 0)
        {
            stxIdx = tcpIn.indexOf(STX);
            etxIdx = tcpIn.indexOf(ETX);
        }
        else
        {
            stxIdx = -1;
            etxIdx = -1;
        }
    }
};

// --------------------------
// Wcs Public Methods
// --------------------------
WCS::WCS()
{
    this->echos = new Echo();
    using namespace std::placeholders;
    this->bindedSender = std::bind(static_cast<bool (WCS::*)(String, bool, bool)>(&WCS::send), this, _1, _2, _3);

    this->lastPingMillis = 0;
    this->pong = false;
    this->droppedPings = 0;
};

bool WCS::init()
{
    bool res = false;
    // rehydrate status
    logMaster("Rehydrating Shuttle Status");
    res = status->rehydrateStatus();
    if (!res)
    {
        logMaster("Failed to complete Rehydration process");
        resetChip();
    }
    else
        logMaster("Status Rehydration Complete");

    // connect to wifi
    String wifiLog = "Connecting to Wifi ";
    wifiLog += ssid;
    logMaster(wifiLog);
    res = ConnectWifi();
    if (!res)
    {
        logMaster("Failed to connect to Wifi");
        resetChip();
    }
    else
        logMaster("Wifi Connected");

    // connect to wcs server
    String wcsLog = "Connecting to WCS Server ";
    wcsLog += serverIp;
    logMaster(wcsLog);
    res = ConnectTcpServer();
    if (!res)
    {
        logMaster("Failed to connect to server");
        resetChip();
    }
    else
        logMaster("Wcs Server Connected");

    // retrieve existing shuttle status
    WcsCommsFormat *wcsLoginMsg = new WcsCommsFormat();
    wcsLoginMsg->id = status->getId();
    wcsLoginMsg->instructions = GET_TWO_DIGIT_STRING(status->getLevel());
    wcsLoginMsg->instructions += GET_TWO_DIGIT_STRING(status->getStringState());
    wcsLoginMsg->actionEnum = GET_TWO_DIGIT_STRING(LOGIN);

    logMaster("Logging in to Server");
    // send login message
    res = this->send(*wcsLoginMsg, true, false);

    // delete login message
    delete wcsLoginMsg;

    // return init control
    return res;
};

void WCS::run()
{
    // check for WCS messages
    if (TcpRead())
        this->handleTcpInput();

    // check pings
    this->runPing();

    // check echos
    this->echos->run(logMasterError, this->bindedSender);
};

void WCS::updateStateChange()
{
    WcsCommsFormat stateChange;
    stateChange.actionEnum = STATE;
    stateChange.instructions = GET_TWO_DIGIT_STRING(status->getState());
    this->send(stateChange);
};

void WCS::notifyTaskCompletion()
{
    WcsCommsFormat taskCompletion;
    taskCompletion.actionEnum = (ENUM_WCS_ACTIONS)status->getActionEnum().toInt();
    taskCompletion.instructions = "success";
    this->send(taskCompletion);

    // update shuttle status to IDLE
    status->setActionEnum = IDLE;
    status->clearInstructions();
    status->saveStatus();
};