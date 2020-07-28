#include "../include/common.h"
#include "./Status/Status.h"
#include "./Helpers/Local/LocalHelper.h"
#include "./WCS/WCS.h"
#include "./Slave/Slave.h"

using namespace Logger;

void setup()
{
  bool initializationRes = false;

  // initialize logger
  initializationRes = initLogger();

  // initialize WCS - connect to wifi and to server
  initializationRes = wcs->init();

  // initialize Slave - start serial to slave chip
  initializationRes = slave->init(&Serial);

  if (!initializationRes)
    while (true)
      ;
}

void loop()
{
  // run codes only if wifi is connected.
  // else wait for wifi reconnection
  if (isWifiConnected())
  {
    Logger::run();
    wcs->run();
    slave->run();
  }
}