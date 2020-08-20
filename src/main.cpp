#include "Wifi/Wifi.h"
#include "Logger/Logger.h"
#include "Status/Status.h"
#include "WCS/WCS.h"
#include "Slave/Slave.h"
#include "ESPWrover/ESPWrover.h"

Slave slave;
WCS wcs;

void setup()
{
  bool initializationRes;
  // Init logger
  initializationRes = Logger::init();

  // init status
  initializationRes = Status::init();

  Logger::log("Connecting to " + Wifi::ssid);
  // connect wifi
  initializationRes = Wifi::ConnectWifi();
  if (!initializationRes)
    Logger::log("Failed to connect to " + Wifi::ssid);
  else
    Logger::log("Wifi Connected to " + Wifi::ssid);

  // init slave
  initializationRes = slave.init(&Serial);
  if (!initializationRes)
    Logger::log("Failed to initialize slave-master");
  else
    Logger::log("Slave-master initialized");

  Logger::log("Connect to server " + TCP::serverIp + ":" + String(TCP::serverPort));
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
  {
    Logger::log("Resetting chip");
    delay(2000);
    ESPWrover::reset();
  }
  else
    Logger::logInitSuccess();
}

void loop()
{
  wcs.run();
  slave.run();
}