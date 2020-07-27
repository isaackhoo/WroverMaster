#include "./Logger.h"

namespace Logger
{
    bool initLogger()
    {
        // initialises lcd and sd card reader
        LcdInit();
        if (!SdInit())
        {
            info("Failed to initialize SD Card");
            return false;
        }
        info("LCD and SD successfully initialized");
        return true;
    };

    void info(const char *cStr)
    {
        LcdScrollText(cStr);
    };

    void info(String str)
    {
        info(str.c_str());
    };

    void logWCS(const char *cStr)
    {
        info(cStr);
        logWcsToSd(cStr);
    };

    void logWCS(String str)
    {
        logWCS(str.c_str());
    };

    void logWCSError(const char *cErr)
    {
        setLcdTextColorError();
        info(cErr);
        logWcsErrorToSd(cErr);
        setLcdTextColorPrimary();
    };

    void logWCSError(String err)
    {
        logWCSError(err.c_str());
    };

    void logMaster(const char *cStr)
    {
        info(cStr);
        logMasterToSd(cStr);
    };

    void logMaster(String str)
    {
        logMaster(str.c_str());
    };

    void logMasterError(const char *cErr)
    {
        setLcdTextColorError();
        info(cErr);
        logMasterErrorToSd(cErr);
        setLcdTextColorPrimary();
    };

    void logMasterError(String err)
    {
        logMasterError(err.c_str());
    };

    void logSlave(const char *cStr)
    {
        info(cStr);
        logSlaveToSd(cStr);
    };

    void logSlave(String str)
    {
        logSlave(str.c_str());
    };

    void logSlaveError(const char *cErr)
    {
        setLcdTextColorError();
        info(cErr);
        logSlaveErrorToSd(cErr);
        setLcdTextColorPrimary();
    };

    void logSlaveError(String err)
    {
        logSlaveError(err.c_str());
    };

    void run()
    {
        sdRun();
    };
} // namespace Logger
