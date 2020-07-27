#include "./LocalEsp.h"

void resetChip()
{
    // terminate tcp
    TcpClose();
    // logToSd();
    ESP.restart();
};