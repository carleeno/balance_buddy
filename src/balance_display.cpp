#include "balance_display.h"

BalanceDisplay::BalanceDisplay(ESC &esc) : esc(&esc) {}

void BalanceDisplay::setup()
{
#ifdef USE_I2C
  oled.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR);
#else
  oled.begin(SSD1306_SWITCHCAPVCC);
#endif
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.println("Startup...");
  oled.display();
}

void BalanceDisplay::clear()
{
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.setFont();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
}

void BalanceDisplay::println(String text)
{
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.println(text);
  oled.display();
}

void BalanceDisplay::loop()
{
  if (lastPageMillis + PAGE_TIME < millis())
  {
    lastPageMillis = millis();
    page++;
    if (page > 2)
    {
      page = 1;
    }
  }
  if (speed > 0.5)
  {
    lastPageMillis = millis();
    page = 1;
  }

  if (lastRenderMillis + REFRESH_INTERVAL < millis())
  {
    lastRenderMillis = millis();
    
    speed = fabs(esc->speed_ms) * speed_conversion;
    if (speed > top_speed)
      top_speed = speed;

    oled.clearDisplay();
    switch (page)
    {
    case 1:
      page1();
      break;
    case 2:
      if (esc->packVoltage > 0)
        page2();
      else
        page1();
      break;
    }

    // Write page to oled
    oled.display();
  }
}

void BalanceDisplay::page1()
{
  oled.setTextColor(SSD1306_WHITE);

  // Line 1: Voltage and Temp
  oled.setCursor(0, 6);
  oled.setFont();
  oled.setTextSize(1);
  if (esc->packVoltage > 0)
  {
    oled.print(esc->packSoC);
    oled.print("% ");
  }
  else
  {
    oled.print(esc->voltage);
    oled.print("V ");
  }
  oled.print(esc->tempMosfet);
  oled.print("c ");
  oled.print(esc->tempMotor);
  oled.print("c");

  // Line 2: ADC Switches
  oled.setCursor(0, 16);
  oled.setFont();
  oled.setTextSize(1);
  oled.print("ADC1: ");
  oled.print(esc->adc1);
  oled.setTextSize(2);
  if (esc->switchState == 0)
    oled.print(" OFF");
  else if (esc->switchState == 1)
    oled.print(" HALF");
  else
    oled.print(" ON");
  oled.setTextSize(1);
  oled.println();
  oled.print("ADC2: ");
  oled.print(esc->adc2);

  // Line 3: Balance state
  oled.setCursor(0, 36);
  oled.setFont();
  if (0 <= esc->balanceState && esc->balanceState < sizeof(balStates))
    oled.print(balStates[esc->balanceState]);
  else
  {
    oled.print("Unknown: ");
    oled.print(esc->balanceState);
  }

  // Line 4: Duty Cycle (up to SCREEN_WIDTH-2 verical bars)
  oled.drawRect(0, 48, SCREEN_WIDTH, 16, SSD1306_WHITE);
  int pos = 1;
  for (double i = dc_step; i <= fabs(esc->dutyCycle); i += dc_step)
  {
    oled.drawFastVLine(pos, 49, 14, SSD1306_WHITE);
    pos++;
  }
  oled.setCursor(2, 52);
  oled.setTextColor(SSD1306_INVERSE);
  if (speed > 0.5)
  {
    oled.print(speed);
    oled.print(speed_unit);
  }
  else
  {
    oled.print("Top speed: ");
    oled.print(top_speed);
    oled.print(speed_unit);
  }
}

void BalanceDisplay::page2()
{
  oled.setTextColor(SSD1306_WHITE);

  // Line 1 pack info:
  oled.setCursor(0, 6);
  oled.setFont();
  oled.setTextSize(1);
  oled.print(esc->packSoC);
  oled.print("% ");
  oled.print(esc->packVoltage);
  oled.print("V ");
  oled.print(esc->packCurrent);
  oled.print("A");

  // Line 2+ cell info:
  oled.setCursor(0, 20);
  oled.print("Highest cell:  ");
  oled.print(esc->cellVoltageHigh);
  oled.println("V");
  oled.print("Cell Avg:      ");
  oled.print(esc->cellVoltageAverage);
  oled.println("V");
  oled.print("Lowest cell:   ");
  oled.print(esc->cellVoltageLow);
  oled.println("V");
  oled.print("Mismatch:      ");
  oled.print(esc->cellVoltageMisMatch);
  oled.print("V");
}
