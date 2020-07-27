#include "../include/common.h"
#include "./Helpers/Local/LocalHelper.h"
#include "./Network/Wifi/WifiNetwork.h"

int counter = 0;

using namespace Logger;

void setup()
{
  bool initializationRes = false;

  // initialise logger
  initializationRes = initLogger();

  // connect to wifi
  if (!ConnectWifi())
  {
    info("Failed to connect to wifi");
  };

  if (!initializationRes)
  {
    while (true)
    {
      // do nothing
    };
  }
}

void loop()
{
  // run codes only if wifi is connected.
  // else wait for wifi reconnection
  if (isWifiConnected())
  {
    Logger::run();

    ++counter;

    logWCS(String(counter));
    logWCSError(String(counter));
    logMaster(String(counter));
    logMasterError(String(counter));
    logSlave(String(counter));
    logSlaveError(String(counter));
    
    delay(100);
  }
}