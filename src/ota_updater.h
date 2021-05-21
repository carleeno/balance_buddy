#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"
#include "balance_display.h"

class otaUpdater
{
private:
  const char *ssid = WIFI_SSID;
  const char *password = WIFI_PASS;

  BalanceDisplay *display;

public:
  otaUpdater(BalanceDisplay &d);

  void setup();

  void loop();
};
