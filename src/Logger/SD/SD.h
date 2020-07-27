#pragma once

#ifndef SD_H
#define SD_H

#include <SD_MMC.h>
#include "../../Network/Wifi/WifiNetwork.h"

extern bool isInit;
extern String LogsDirectory;
extern String StatusDirectory;
extern String StatusFilePath;
extern String currentFileName;

extern unsigned long lastTimestampMillis;
extern const unsigned long updateTimestampInterval;
extern unsigned long lastDateMillis;
extern const unsigned long updateDateInterval;

// sd public methods
extern bool SdInit();
extern bool logToSd(const char *, const char *, const char *);
extern bool logToSd(const char *);
extern bool logSystemToSd(String);

extern bool logWcsToSd(String);
extern bool logWcsErrorToSd(String);
extern bool logMasterToSd(String);
extern bool logMasterErrorToSd(String);
extern bool logSlaveToSd(String);
extern bool logSlaveErrorToSd(String);
extern void sdRun();

extern void logStatus(String);
extern String readStatus();

#endif