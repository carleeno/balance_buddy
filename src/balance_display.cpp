#include <Adafruit_SSD1306.h>
#include "config.h"
// Fonts:
#include <Fonts/FreeSerif9pt7b.h>

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
  void setup()
  {
#ifdef USE_I2C
    oled.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR);
#else
    oled.begin(SSD1306_SWITCHCAPVCC);
#endif
  }

  void loop(double tempMosfet, double tempMotor, double dutyCycle, double voltage, uint16_t balanceState, uint16_t switchState, double adc1, double adc2)
  {
    if (lastRenderMillis + REFRESH_INTERVAL < millis())
    {
      lastRenderMillis = millis();

      oled.clearDisplay();
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(0, 6);

      // Line 1: Voltage and Temp
      oled.setFont();
      oled.setTextSize(1);
      oled.print(voltage);
      oled.print("V ");
      oled.print(tempMosfet);
      oled.print("c ");
      oled.print(tempMotor);
      oled.print("c");

      // Line 2: ADC Switches
      oled.setCursor(0, 16);
      oled.setFont();
      oled.setTextSize(1);
      oled.print("ADC1: ");
      oled.print(adc1);
      oled.setTextSize(2);
      if (switchState == 0)
      {
        oled.print(" OFF");
      }
      else if (switchState == 1)
      {
        oled.print(" HALF");
      }
      else
      {
        oled.print(" ON");
      }
      oled.setTextSize(1);
      oled.println();
      oled.print("ADC2: ");
      oled.print(adc2);

      // Line 3: Balance state
      oled.setCursor(0, 36);
      oled.setFont();
      if (balanceState == 0)
      {
        oled.print("Calibrating");
      }
      else if (balanceState == 1)
      {
        oled.print("Running");
      }
      else if (balanceState == 2)
      {
        oled.print("Run: Tiltback Duty");
      }
      else if (balanceState == 3)
      {
        oled.print("Run: Tiltback HV");
      }
      else if (balanceState == 4)
      {
        oled.print("Run: Tiltback LV");
      }
      else if (balanceState == 5)
      {
        oled.print("Run: Tilt Constant");
      }
      else if (balanceState == 6)
      {
        oled.print("Fault: Pitch Angle");
      }
      else if (balanceState == 7)
      {
        oled.print("Fault: Roll Angle");
      }
      else if (balanceState == 8)
      {
        oled.print("Fault: Switch Half");
      }
      else if (balanceState == 9)
      {
        oled.print("Fault: Switch Full");
      }
      else if (balanceState == 10)
      {
        oled.print("Fault: Duty");
      }
      else if (balanceState == 11)
      {
        oled.print("Initial");
      }
      else
      {
        oled.print("Unknown: ");
        oled.print(balanceState);
      }

      // Line 4: Duty Cycle (up to SCREEN_WIDTH-2 verical bars)
      oled.drawRect(0, 48, SCREEN_WIDTH, 16, SSD1306_WHITE);
      int pos = 1;
      for (double i = dc_step; i <= fabs(dutyCycle); i += dc_step)
      {
        oled.drawFastVLine(pos, 49, 14, SSD1306_WHITE);
        pos++;
      }
      oled.setCursor(2, 52);
      oled.setTextColor(SSD1306_INVERSE);
      oled.print(int (fabs(dutyCycle) * 100));
      oled.print("%");

      // Write to oled
      oled.display();
    }
  }
};
