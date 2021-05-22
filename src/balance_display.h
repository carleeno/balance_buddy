#pragma once
#include <Adafruit_SSD1306.h>
#include "config.h"

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
  int line = 1;
  double dc_step = 1.0 / (SCREEN_WIDTH - 2);
  double speed;
  double top_speed;

#ifdef USE_MPH
  double speed_conversion = 2.23694;
  String speed_unit = "mph";
#else
  double speed_conversion = 3.6;
  String speed_unit = "km/h";
#endif

public:
  void setup();

  void clear();

  void println(String text);

  void loop(double speed_ms, double tempMosfet, double tempMotor, double dutyCycle, double voltage, uint16_t balanceState, uint16_t switchState, double adc1, double adc2);
};
