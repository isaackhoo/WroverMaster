#include "common.h"
#include "controlCharacters.h"
#include "Helpers/Local/LocalHelper.h"
#include "Status/Status.h"
#include "WCS/WCS.h"
#include "Slave/Slave.h"

using namespace Logger;

const char *SOH = "\x01";
const char *STX = "\x02";
const char *ETX = "\x03";
const char *EOT = "\x04";
const char *NAK = "\x25";

Status *status;
WCS *wcs;
Slave *slave;

void setup()
{
  bool initializationRes = false;

  // initialize logger
  initializationRes = initLogger();

  // create shuttle status
  status = new Status();

  // initialize WCS - connect to wifi and to server
  wcs = new WCS();
  initializationRes = wcs->init(status);

  // bind wcs to status
  status->setWcsContext(wcs);

  // initialize Slave - start serial to slave chip
  slave = new Slave();
  initializationRes = slave->init(&Serial, status);

  // bind slave to wcs
  wcs->setSlaveInstance(slave);
  // bind wcs to slave
  slave->setWcsInstance(wcs);

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