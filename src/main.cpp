#include "Wifi/Wifi.h"
#include "Logger/Logger.h"
#include "Status/Status.h"
#include "WCS/WCS.h"

WCS wcs;

void setup()
{
  bool initializationRes;
  // Init logger
  initializationRes = Logger::init();

  // init status
  initializationRes = Status::init();

  // connect wifi
  initializationRes = Wifi::ConnectWifi();
  if (!initializationRes)
    Logger::log("Failed to connect to " + Wifi::ssid);
  else
    Logger::log("Wifi Connected to " + Wifi::ssid);

  // init wcs
  initializationRes = wcs.init();
  if (!initializationRes)
    Logger::log("Failed to connect to " + TCP::serverIp);
  else
    Logger::log("Connected to " + TCP::serverIp);

  if (!initializationRes)
    while (true)
      ;
  else
    Logger::logInitSuccess();
}

void loop()
{
  wcs.run();
}