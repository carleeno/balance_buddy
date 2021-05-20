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
  otaUpdater(BalanceDisplay &d):display(&d) {}

  void setup()
  {
    display->println("Wifi Connecting...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
      ArduinoOTA.setPort(3232);
      ArduinoOTA.setHostname(HOSTNAME);

      // No authentication by default
      // ArduinoOTA.setPassword("admin");

      // Password can be set with it's md5 value as well
      // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
      // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

      ArduinoOTA
          .onStart([&]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
              type = "sketch";
            else // U_SPIFFS
              type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            display->clear();
            display->println("OTA UPDATE!!!");
          })
          .onEnd([&]() {
            display->clear();
            display->println("OTA UPDATE!!!");
            display->println("Finished");
          })
          .onProgress([&](unsigned int progress, unsigned int total) {
            display->clear();
            display->println("OTA UPDATE!!!");
            display->println("Progress: " + String(progress / (total / 100)) + "%");
          })
          .onError([&](ota_error_t error) {
            display->clear();
            display->println("OTA UPDATE!!!");
            display->println("Error: " + String(error));
            if (error == OTA_AUTH_ERROR)
              display->println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
              display->println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
              display->println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
              display->println("Receive Failed");
            else if (error == OTA_END_ERROR)
              display->println("End Failed");
          });

      ArduinoOTA.begin();

      display->clear();
      display->println("Connected to wifi:");
      display->println(WiFi.localIP().toString());
      display->println(HOSTNAME);
      delay(5000);
    }
    else
    {
      display->println("Unable to connect to wifi");
      display->println("Restart to retry wifi");
      delay(2000);
    }
  }

  void loop()
  {
    ArduinoOTA.handle();
  }
};
