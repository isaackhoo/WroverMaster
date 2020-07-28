#pragma once

#ifndef WCS_H
#define WCS_H

#include "../include/common.h"
#include "../include/controlCharacters.h"
#include "./Status/Status.h"
#include "./Logger/Logger.h"
#include "./Logger/SD/SD.h"
#include "./Network/TCP/TCP.h"
#include "./Network/Wifi/WifiNetwork.h"
#include "./Helpers/Local/LocalHelper.h"
#include "./Helpers/ESP/LocalEsp.h"
#include "./Echo/Echo.h"


using namespace Logger;

// --------------------------
// WCS Private Variables
// --------------------------
extern const unsigned long PING_INTERVAL;
extern const unsigned long PING_DROPPED_INTERVAL;
extern const unsigned int MAX_DROPPED_PINGS;

extern const unsigned long ECHO_TIMEOUT_DURATION;

enum ENUM_WCS_ACTIONS
{
    NONE = 0,
    LOGIN,       // 1
    LOGOUT,      // 2
    PING,        // 3
    RETRIEVEBIN, // 4
    STOREBIN,    // 5
    MOVE,        // 6
    BATTERY,     // 7
    STATE,       // 8
    LEVEL,       // 9
    SET,         // 10
    ECHO,        // 11
    ERROR,       // 12
    Num_Of_WCS_Action_Enums
};

enum ENUM_MANUAL_SET_TYPES
{
    MANUAL_SET_DEFAULT = 0,
    MANUAL_SET_ID,    // 01
    MANUAL_SET_LEVEL, // 02
    MANUAL_SET_STATE, // 03
    MANUAL_SET_ECHO,  // 04
    Num_Of_Manual_Set_Types_Enums
};

const int DEFAULT_ENUM_VALUE_LENGTH = 2;

struct WcsCommsFormat
{
    String id;           // shuttle 4 digit ID
    String actionEnum;   // 2 digit action enum
    String instructions; // variable length string
};

// --------------------------
// WCS Public Variables
// --------------------------

// --------------------------
// WCS Public Methods
// --------------------------
class WCS
{
private:
    Echo *echos;
    SenderFunction bindedSender;

    bool pong;
    bool pongChecked;
    unsigned int lastPingMillis;
    unsigned int droppedPings;

    String createSendString(WcsCommsFormat);
    String createSendString(WcsCommsFormat, bool);
    bool send(String, bool, bool);
    bool send(WcsCommsFormat, bool, bool);
    bool send(WcsCommsFormat, bool);
    bool send(WcsCommsFormat);

    void pingServer();
    void runPing();
    void startPings();

    WcsCommsFormat *interpret(String);
    void updatePingReceived();
    void perform(WcsCommsFormat *);
    void handleTcpInput();

public:
    WCS();
    bool init();
    void run();
    bool updateStateChange();
};

// export only a single instance of WCS
extern WCS *wcs;

#endif