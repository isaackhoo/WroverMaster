#include "./WCS.h"

// --------------------------
// Wcs Public Variables
// --------------------------
WCS wcs;

// --------------------------
// Wcs Private Methods
// --------------------------
bool WCS::send(WcsCommsFormat sendObj, bool shouldLog, bool awaitEcho)
{
    // Send format
    // 1    - STX
    // 2-5  - shuttle ID
    // 6-7  - shuttle Action
    // 8-x  - other information
    // x+1  - ETX

    String sendString = "";
    sendString += STX;
    sendString += sendObj.id;
    sendString += sendObj.actionEnum;
    sendString += sendObj.instructions;
    sendString += ETX;

    bool res = TcpWrite(sendString.c_str());

    String sendLog = "";
    if (!res)
        sendLog = "Failed to send to server";
    else
    {
        sendLog = "<< ";
        sendLog += sendString;
    }

    if (shouldLog)
        logWCS(sendLog);

    if (awaitEcho)
    {
        // TODO
    }
};
bool WCS::send(WcsCommsFormat sendObj, bool shouldLog)
{
    this->send(sendObj, shouldLog, true);
};
bool WCS::send(WcsCommsFormat sendObj)
{
    this->send(sendObj, true, true);
};

// --------------------------
// Wcs Public Methods
// --------------------------
WCS::WCS()
{
}

void WCS::init()
{
    // rehydrate status
    logMaster("Rehydrating Status...");
    if (status.rehydrateStatus())
        logMaster("Status Rehydration Complete");

    // connect to wifi
    String wifiLog = "Connecting to Wifi ";
    wifiLog += ssid;
    logMaster(wifiLog);
    if (!ConnectWifi())
    {
        logMaster("Failed to connect to Wifi");
        resetChip();
    }
    logMaster("Wifi Connected");

    // connect to wcs server
    String wcsLog = "Connecting to WCS Server ";
    wcsLog += serverIp;
    logMaster(wcsLog);
    if (!ConnectTcpServer())
    {
        logMaster("Failed to connect to server");
        resetChip();
    }
    logMaster("Wcs Server Connected");

    // retrieve existing shuttle status
    WcsCommsFormat wcsLoginMsg;
    wcsLoginMsg.id = status.getId();
    wcsLoginMsg.instructions = status.getLevel();
    wcsLoginMsg.instructions += status.getStringState();
    wcsLoginMsg.actionEnum = GET_TWO_DIGIT_STRING(LOGIN);

    // send login message
    this->send(wcsLoginMsg);
}

void WCS::run()
{
}