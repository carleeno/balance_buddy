#pragma once
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "esc.h"

class BalanceDisplay
{
private:
#ifdef USE_I2C
  Adafruit_SSD1306 oled{SCREEN_WIDTH, SCREEN_HEIGHT, &Wire};
#else
  Adafruit_SSD1306 oled{SCREEN_WIDTH, SCREEN_HEIGHT,
                        &SPI, OLED_DC, OLED_RESET, OLED_CS};
#endif
  long lastRenderMillis = 0;
  int page = 1;
  long lastPageMillis = 0;
  double dc_step = 1.0 / (SCREEN_WIDTH - 2);
  double speed;
  double top_speed;

  const String balStates[12]{
      "Calibrating",
      "Running",
      "Run: Tiltback Duty",
      "Run: Tiltback HV",
      "Run: Tiltback LV",
      "Run: Tilt Constant",
      "Fault: Pitch Angle",
      "Fault: Roll Angle",
      "Fault: Switch Half",
      "Fault: Switch Full",
      "Fault: Duty",
      "Initial"};

#ifdef USE_MPH
  double speed_conversion = 2.23694;
  String speed_unit = "mph";
#else
  double speed_conversion = 3.6;
  String speed_unit = "km/h";
#endif

  ESC *esc;

  void page1();

  void page2();

public:
  BalanceDisplay(ESC &esc);

  void setup();

  void clear();

  void println(String text);

  void loop();
};
