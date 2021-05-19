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

public:
  void setup()
  {
#ifdef USE_I2C
    oled.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR);
#else
    oled.begin(SSD1306_SWITCHCAPVCC);
#endif
  }

  void loop(double tempMosfet, double dutyCycle, double voltage, uint16_t balanceState, uint16_t switchState, double adc1, double adc2)
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
      oled.print("V");
      oled.print("  ");
      oled.print(tempMosfet);
      oled.println("c");

      // Line 2: ADC Switches
      oled.setCursor(0, 12);
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
      oled.println(adc2);

      // Line 3: Balance state
      oled.setCursor(0, 32);
      oled.setFont();
      if (balanceState == 0)
      {
        oled.println("Calibrating");
      }
      else if (balanceState == 1)
      {
        oled.println("Running");
      }
      else if (balanceState == 2)
      {
        oled.println("Run: Tiltback Duty");
      }
      else if (balanceState == 3)
      {
        oled.println("Run: Tiltback HV");
      }
      else if (balanceState == 4)
      {
        oled.println("Run: Tiltback LV");
      }
      else if (balanceState == 5)
      {
        oled.println("Run: Tilt Constant");
      }
      else if (balanceState == 6)
      {
        oled.println("Fault: Pitch Angle");
      }
      else if (balanceState == 7)
      {
        oled.println("Fault: Roll Angle");
      }
      else if (balanceState == 8)
      {
        oled.println("Fault: Switch Half");
      }
      else if (balanceState == 9)
      {
        oled.println("Fault: Switch Full");
      }
      else if (balanceState == 10)
      {
        oled.println("Fault: Duty");
      }
      else if (balanceState == 11)
      {
        oled.println("Initial");
      }
      else
      {
        oled.print("Unknown: ");
        oled.print(balanceState);
        oled.println("");
      }

      // Line 4: Duty Cycle (up to 64 verical bars)
      oled.setCursor(0, 52);
      oled.setFont(&FreeSerif9pt7b);
      for (float i = 0; i < fabsf(dutyCycle); i += 0.0313)
      {
        oled.print("|");
      }

      // Write to oled
      oled.display();
    }
  }
};
