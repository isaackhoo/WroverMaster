#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "LCD/LCD.h"
#include "SD/SD.h"

namespace Logger
{
    extern bool init();
    extern void logInitSuccess();
    extern void log(String);
    extern void logError(String);

    extern void logWcs(String);
    extern void logWcsError(String);

    extern void logSlave(String);
    extern void logSlaveError(String);
    extern void run();
}; // namespace Logger

#endif