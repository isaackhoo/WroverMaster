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
        // does not cause heap fragmentation. tested
        tcpIn = client.readString();
        tcpIn.trim();
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
    TcpWrite(str.c_str());
};

bool TcpClose()
{
    client.stop();
    if (client.connected())
        return false;
    return true;
}