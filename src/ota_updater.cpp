#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"
#include "balance_display.cpp"

class otaUpdater
{
private:
  const char *ssid = WIFI_SSID;
  const char *password = WIFI_PASS;

  BalanceDisplay *display; // Here I have no idea what I'm doing lol

public:
  otaUpdater(BalanceDisplay display) // trying to take instance of BalanceDisplay passed from main.cpp
  {
    display = display;
  }
  void setup()
  {
    display.println("Wifi Connecting..."); // I want to replace all Serial.println with display.println from BalanceDisplay so it prints to OLED
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
          .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
              type = "sketch";
            else // U_SPIFFS
              type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
          })
          .onEnd([]() {
            Serial.println("Finished");
          })
          .onProgress([](unsigned int progress, unsigned int total) {
            Serial.println("Progress: " + progress / (total / 100));
          })
          .onError([](ota_error_t error) {
            Serial.println("Error: " + error);
            if (error == OTA_AUTH_ERROR)
              Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
              Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
              Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
              Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR)
              Serial.println("End Failed");
          });

      ArduinoOTA.begin();

      Serial.println("Ready");
      Serial.println("IP address:");
      Serial.println(WiFi.localIP().toString());
      Serial.println("Hostname:");
      Serial.println(HOSTNAME);
    }
    else
    {
      Serial.println("Unable to connect to wifi");
      Serial.println("Restart to retry wifi");
    }
  }

  void loop()
  {
    ArduinoOTA.handle();
  }
};
