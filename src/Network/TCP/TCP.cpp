#include "./TCP.h"

// ----------------------------
// TCP Client Variables
// ----------------------------
WiFiClient client;
String tcpIn = "";

// --------------------------
// TCP Client Public Methods
// --------------------------
bool ConnectTcpServer()
{
    if (!client.connect(serverIp.c_str(), serverPort))
        return false;
    return true;
};

bool TcpRead()
{
    if (client.available())
    {
        // tcp buffer has data. Read data
        tcpIn += client.readString();
        return true;
    }
    return false;
};

bool TcpWrite(const char *toWrite)
{
    if (GET_ARRAY_SIZE(toWrite) <= 0)
        return false;
    if (!client.connected())
        return false;
    client.print(toWrite);
    return true;
};

bool TcpWrite(String str)
{
    return TcpWrite(str.c_str());
};

bool TcpClose()
{
    client.stop();
    if (client.connected())
        return false;
    return true;
}