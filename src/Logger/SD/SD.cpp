#include "Logger/SD/SD.h"

namespace SDCard
{
    // -----------------------------
    // SD CONSTANTS DECLARATION
    // -----------------------------
    bool isInit = false;
    const String LogsDirectory = F("/Logs");
    const String StatusDirectory = F("/Status");
    const String StatusFilePath = StatusDirectory + "/status.txt";
    String currentFileName = "";

    unsigned long lastTimestampMillis = 0;
    const unsigned long updateTimestampInterval = 1000 * 15; //15s
    unsigned long lastDateMillis = 0;
    const unsigned long updateDateInterval = 1000 * 60 * 30; // 30 mins

    const String COLOR_ERROR = F("rgba(226,6,6,1)");
    const String COLOR_DEFAULT = F("rgba(0,0,0,1)");

    const String TEXT_ALIGN_LEFT = F("left");
    const String TEXT_ALIGN_CENTER = F("center");
    const String TEXT_ALIGN_RIGHT = F("right");

    // -----------------------------
    // SD PRIVATE METHODS
    // -----------------------------
    bool createDir(const char *path)
    {
        if (SD_MMC.mkdir(path))
            return true;
        else
            return false;
    }

    bool removeDir(const char *path)
    {
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

    bool logToSd(String color, String textAlign, String str)
    {
        if (str.length() <= 0)
            return false;

        String htmlLogMsg((char *)0);
        htmlLogMsg.reserve(256);
        htmlLogMsg += F("<div style=\"width:100%; color:");
        htmlLogMsg += color;
        htmlLogMsg += F("; text-align:");
        htmlLogMsg += textAlign;
        htmlLogMsg += F(";\">");
        htmlLogMsg += str;
        htmlLogMsg += F("<br></div>");

        // append input to logs file
        if (!appendFile(getLatestLogPath().c_str(), htmlLogMsg.c_str()))
            return false;

        return true;
    };

    bool logSystemToSd(String str)
    {
        return logToSd(COLOR_DEFAULT, TEXT_ALIGN_CENTER, str.c_str());
    };

    void logTimestamp()
    {
        String timestamp((char *)0);
        timestamp.reserve(16); 
        timestamp += F("["); 
        timestamp += getCurrentTime(); 
        timestamp += F("]\n");
        logSystemToSd(timestamp);
    };

    // -----------------------------
    // SD PUBLIC METHODS
    // -----------------------------
    bool sdInit()
    {
        if (SD_MMC.begin())
        {
            // create logs directory if it does not exist
            createDir(LogsDirectory.c_str());

            // indicate that SD has been initialised
            isInit = true;
        }
        return isInit;
    };

    bool logWcsToSd(String str)
    {
        return logToSd(COLOR_DEFAULT, TEXT_ALIGN_LEFT, str.c_str());
    };

    bool logWcsErrorToSd(String err)
    {
        return logToSd(COLOR_ERROR, TEXT_ALIGN_LEFT, err.c_str());
    };

    bool logMasterToSd(String str)
    {
        return logToSd(COLOR_DEFAULT, TEXT_ALIGN_CENTER, str.c_str());
    };

    bool logMasterErrorToSd(String err)
    {
        return logToSd(COLOR_ERROR, TEXT_ALIGN_CENTER, err.c_str());
    };

    bool logSlaveToSd(String str)
    {
        return logToSd(COLOR_DEFAULT, TEXT_ALIGN_RIGHT, str.c_str());
    };

    bool logSlaveErrorToSd(String err)
    {
        return logToSd(COLOR_ERROR, TEXT_ALIGN_RIGHT, err.c_str());
    };

    void run()
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

    void setStatus(String status)
    {
        // creates the status directory if it doesnt already exist
        createDir(StatusDirectory.c_str());
        // write status to file
        writeFile(StatusFilePath.c_str(), status.c_str());
        // log
        String output((char *)0);
        output.reserve(64); 
        output += F("Status updated: ");
        output += status;
        logMasterToSd(output);
    };

    String readStatus()
    {
        return readFile(StatusFilePath.c_str());
    };
}; // namespace SD
