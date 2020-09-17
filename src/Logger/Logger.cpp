#include "Logger/Logger.h"

namespace Logger
{
    // -------------------------------
    // LOGGER PRIVATE VARIABLES
    // -------------------------------

    // -------------------------------
    // LOGGER PUBLIC METHODS
    // -------------------------------
    bool init()
    {
        bool res;
        res = LCD::init();
        if (res)
            LCD::display(F("LCD initialized"));
        res = SDCard::sdInit();
        if (res)
            LCD::display(F("SD initialized"));
        else
            LCD::display(F("SD faild to initialize"));
        return res;
    };

    void logInitSuccess()
    {
        SDCard::logMasterToSd(F("----------------------------- Master Initialized -----------------------------"));
    };

    void log(String output)
    {
        LCD::display(output);
        SDCard::logMasterToSd(output);
    };

    void logError(String err)
    {
        LCD::displayError(err);
        SDCard::logMasterErrorToSd(err);
    };

    void logWcs(String output)
    {
        LCD::display(output);
        SDCard::logWcsToSd(output);
    };

    void logWcsError(String err)
    {
        LCD::displayError(err);
        SDCard::logWcsErrorToSd(err);
    };

    void logSlave(String output)
    {
        LCD::display(output);
        SDCard::logSlaveToSd(output);
    };

    void logSlaveError(String err)
    {
        LCD::displayError(err);
        SDCard::logSlaveErrorToSd(err);
    };

    void run()
    {
        SDCard::run();
    };

    // -------------------------------
    // LOGGER PRIVATE METHODS
    // -------------------------------
}; // namespace Logger