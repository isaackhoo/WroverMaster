#pragma once

#ifndef TCP_H
#define TCP_H

#include <Arduino.h>
#include <WiFiClient.h>
#include "Helper/Helper.h"

// TCPKeys.cpp
// #include "WCS/TCP/TCP.h"

// namespace TCP
// {
//     const String serverIp = "192.168.1.1";
//     const int serverPort = 45555;
// }; // namespace TCP

namespace TCP
{
    // --------------------------
    // TCP Client Variables
    // --------------------------
    extern const String serverIp;
    extern const int serverPort;

    extern String tcpIn;

    // --------------------------
    // TCP Client Public Methods
    // --------------------------
    extern bool init();
    extern bool ConnectTcpServer();
    extern bool TcpRead(const char);
    extern bool TcpWrite(String);
    extern bool TcpClose();
}; // namespace TCP

#endif