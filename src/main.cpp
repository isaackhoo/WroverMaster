#include "Wifi/Wifi.h"
#include "Logger/Logger.h"
#include "Status/Status.h"
#include "WCS/WCS.h"
#include "Slave/Slave.h"
#include "ESPWrover/ESPWrover.h"
#include "Slave/Reset/Reset.h"

Slave slave;
WCS wcs;
Reset reset;

void setup()
{
  bool initializationRes;
  // Init logger
  initializationRes = Logger::init();

  // init status
  if (initializationRes)
  {
    initializationRes = Status::init();
    if (!initializationRes)
    {
      Logger::log("No status file found");
      // create default status folder
      Status::saveStatus();
      Logger::log("Created Status folder");
      Logger::log("Please set ID via SD card");
    } else {
      // check that id is not default id
      if (Status::getId() == DEFAULT_ID) {
        initializationRes = false;
        Logger::logError("Please set shuttle ID");
      }
    };
  }

  if (initializationRes)
  {
    // connect wifi
    Logger::log("Connecting to " + Wifi::ssid);
    initializationRes = Wifi::ConnectWifi();
    if (!initializationRes)
      Logger::log("Failed to connect to " + Wifi::ssid);
    else
      Logger::log("Wifi Connected to " + Wifi::ssid);
  }

  if (initializationRes)
  {
    Logger::log("Resetting Slave");
    reset.resetSlave();
    delay(2000);

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
  }

  if (initializationRes)
  {
    // pass references
    slave.setWcsInstance(&wcs);
    wcs.setSlaveInstance(&slave);
  }

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