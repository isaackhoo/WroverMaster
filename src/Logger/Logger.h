#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include "LCD/LCD.h"
#include "SD/SD.h"

namespace Logger
{
    extern bool initLogger();
    extern void info(const char *);
    extern void info(String);

    extern void logWCS(const char *);
    extern void logWCS(String);
    extern void logWCSError(const char *);
    extern void logWCSError(String);

    extern void logMaster(const char *);
    extern void logMaster(String);
    extern void logMasterError(const char *);
    extern void logMasterError(String);

    extern void logSlave(const char *);
    extern void logSlave(String);
    extern void logSlaveError(const char *);
    extern void logSlaveError(String);

    extern void run();
} // namespace Logger

#endif