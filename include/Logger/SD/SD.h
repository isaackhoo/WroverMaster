#pragma once

#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SD_MMC.h>
#include "Wifi/Wifi.h"

using namespace Wifi;

namespace SDCard
{
    extern bool sdInit();
    extern bool logWcsToSd(String);
    extern bool logWcsErrorToSd(String);
    extern bool logMasterToSd(String);
    extern bool logMasterErrorToSd(String);
    extern bool logSlaveToSd(String);
    extern bool logSlaveErrorToSd(String);
    extern void run();

    extern void setStatus(String);
    extern String readStatus();
}; // namespace SDCard

#endif