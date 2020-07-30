#include "./SD.h"

// -----------------------------
// SD CONSTANTS DECLARATION
// -----------------------------
bool isInit = false;
String LogsDirectory = "/Logs";
String StatusDirectory = "/Status";
String StatusFilePath = StatusDirectory + "/status.txt";
String currentFileName = "";

unsigned long lastTimestampMillis = 0;
const unsigned long updateTimestampInterval = 1000 * 15; //15s
unsigned long lastDateMillis = 0;
const unsigned long updateDateInterval = 1000 * 60 * 30; // 30 mins

// -----------------------------
// SD PRIVATE METHODS
// -----------------------------
bool createDir(const char *path)
{
    if (!isInit)
        return false;
    if (SD_MMC.mkdir(path))
        return true;
    else
        return false;
}

bool removeDir(const char *path)
{
    if (!isInit)
        return false;
    if (SD_MMC.rmdir(path))
        return true;
    else
        return false;
}

String readFile(const char *path)
{
    if (!isInit)
        return "";
    String fileOutput = "";
    File file = SD_MMC.open(path);
    if (file)
    {
        while (file.available())
        {
            fileOutput += (char)file.read();
        }
    }
    file.close();

    return fileOutput;
}

bool writeFile(const char *path, const char *message)
{
    if (!isInit)
        return false;
    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file)
        return false;

    bool res = false;
    if (file.print(message))
        res = true;

    file.close();
    return res;
}

bool appendFile(const char *path, const char *message)
{
    if (!isInit)
        return false;
    File file = SD_MMC.open(path, FILE_APPEND);
    if (!file)
        return false;

    bool res = false;
    if (file.print(message))
        res = true;

    file.close();
    return res;
}

bool renameFile(const char *path1, const char *path2)
{
    if (!isInit)
        return false;
    if (SD_MMC.rename(path1, path2))
        return true;
    else
        return false;
}

bool deleteFile(const char *path)
{
    if (!isInit)
        return false;
    if (SD_MMC.remove(path))
        return true;
    else
        return false;
}

// -----------------------------
// SD HELPER METHODS
// -----------------------------
String getLatestLogFilename()
{
    // iniitalize current file name with current date
    // should the system run overnight, the date will be updated in SD run function
    if (currentFileName == "")
    {
        currentFileName = getCurrentDate();
        currentFileName += ".html";
    }

    return currentFileName;
}

String getLatestLogPath()
{
    String fileName = getLatestLogFilename();
    String writePath = LogsDirectory + "/" + fileName;

    return writePath;
}

// -----------------------------
// SD PUBLIC METHODS
// -----------------------------
bool logToSd(const char *color, const char *textAlign, const char *cStr)
{
    if (GET_ARRAY_SIZE(cStr) <= 0)
        return false;

    String htmlLogMsg = "<div style=\"width:100%; color:";
    htmlLogMsg += String(color);
    htmlLogMsg += "; text-align:";
    htmlLogMsg += String(textAlign);
    htmlLogMsg += ";\">";
    htmlLogMsg += String(cStr);
    htmlLogMsg += "<br></div>";

    // append input to logs file
    if (!appendFile(getLatestLogPath().c_str(), htmlLogMsg.c_str()))
        return false;

    return true;
};

bool logSystemToSd(String str)
{
    return logToSd("rgba(0,0,0,1)", "center", str.c_str());
};

bool logWcsToSd(String str)
{
    return logToSd("rgba(5,2,138,1)", "left", str.c_str());
};

bool logWcsErrorToSd(String err)
{
    return logToSd("rgba(226,6,6,1)", "left", err.c_str());
};

bool logMasterToSd(String str)
{
    return logToSd("rgba(0,0,0,1)", "center", str.c_str());
};

bool logMasterErrorToSd(String err)
{
    return logToSd("rgba(226,6,6,1)", "center", err.c_str());
};

bool logSlaveToSd(String str)
{
    return logToSd("rgba(37,125,207,1)", "right", str.c_str());
};

bool logSlaveErrorToSd(String err)
{
    return logToSd("rgba(226,6,6,1)", "right", err.c_str());
};

void logTimestamp()
{
    String timestamp = "[" + getCurrentTime() + "]\n";
    logSystemToSd(timestamp);
}

bool SdInit()
{
    if (SD_MMC.begin())
    {
        // create logs directory if it does not exist
        createDir(LogsDirectory.c_str());

        // indicate that SD has been initialised
        isInit = true;

        // mark in file new initialization
        logSystemToSd("----------------------------- SD Initialized -----------------------------");
        logTimestamp();
    }
    return isInit;
};

void sdRun()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastTimestampMillis >= updateTimestampInterval)
    {
        lastTimestampMillis = currentMillis;
        logTimestamp();
    }

    if (currentMillis - lastDateMillis >= updateDateInterval)
    {
        lastDateMillis = currentMillis;
        String date = getCurrentDate();
        if (currentFileName != date)
            currentFileName = date;
    }
};

void logStatus(String status)
{
    // creates the status directory if it doesnt already exist
    createDir(StatusDirectory.c_str());
    // write status to file
    writeFile(StatusFilePath.c_str(), status.c_str());
    // log
    String output = "Status updated: ";
    output += status;
    logMasterToSd(output);
};

String readStatus()
{
    return readFile(StatusFilePath.c_str());
};