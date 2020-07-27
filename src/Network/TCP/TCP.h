#pragma once

#ifndef TCP_H
#define TCP_H

#include "../../Logger/Logger.h"
#include <WiFiClient.h>

// const String serverIp = "192.168.1.1"; // actual server static IP;
// const int serverPort = 55555;

// const String serverIp = "192.168.201.45"; // actual server static IP;
// const int serverPort = 55555;

// --------------------------
// TCP Client Variables
// --------------------------
extern WiFiClient client;

extern const String serverIp;
extern const int serverPort;

extern String tcpIn;

// --------------------------
// TCP Client Public Methods
// --------------------------
extern bool ConnectTcpServer();
extern bool TcpRead();
extern bool TcpWrite(const char*);
extern bool TcpWrite(String);
extern bool TcpClose();

#endif