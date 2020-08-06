#include "Wifi/Wifi.h"
#include "Logger/Logger.h"
#include "Status/Status.h"
#include "WCS/WCS.h"
#include "Slave/Slave.h"

Slave slave;
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

  // init slave
  initializationRes = slave.init(&Serial);
  if (!initializationRes)
    Logger::log("Failed to initialize slave");
  else
    Logger::log("Slave initialized");

  // init wcs
  initializationRes = wcs.init();
  if (!initializationRes)
    Logger::log("Failed to connect to " + TCP::serverIp);
  else
    Logger::log("Connected to " + TCP::serverIp);

  // pass references
  slave.setWcsInstance(&wcs);
  wcs.setSlaveInstance(&slave);

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