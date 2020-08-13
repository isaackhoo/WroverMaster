#include "ESPWrover/ESPWrover.h"

namespace ESPWrover
{
    void reset()
    {
        delay(5000);
        ESP.restart();
    };
};