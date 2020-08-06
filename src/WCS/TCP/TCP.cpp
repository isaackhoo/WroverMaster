#include "WCS/TCP/TCP.h"

namespace TCP
{
    // ----------------------------
    // TCP Client Variables
    // ----------------------------
    namespace
    {
        WiFiClient client;
    }; // namespace
    String tcpIn((char *)0);

    // --------------------------
    // TCP Client Public Methods
    // --------------------------
    bool init()
    {
        tcpIn.reserve(256);
        return true;
    };

    bool ConnectTcpServer()
    {
        if (!client.connect(serverIp.c_str(), serverPort))
            return false;
        return true;
    };

    bool TcpRead(const char END_CHAR)
    {
        if (client.available())
        {
            // tcp buffer has data. Read data
            tcpIn += client.readStringUntil(END_CHAR);
            tcpIn += END_CHAR;
            return true;
        }
        return false;
    };

    bool TcpWrite(String str)
    {
        if (str.length() <= 0)
            return false;
        if (!client.connected())
            return false;
        client.print(str);
        return true;
    };

    bool TcpClose()
    {
        client.stop();
        if (client.connected())
            return false;
        return true;
    }
}; // namespace TCP
