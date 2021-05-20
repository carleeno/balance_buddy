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

public:
  void setup();

  void clear();

  void println(String text);

  void loop(double tempMosfet, double tempMotor, double dutyCycle, double voltage, uint16_t balanceState, uint16_t switchState, double adc1, double adc2);
};
