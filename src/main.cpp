#include <Arduino.h>
#include "ota_updater.cpp"
#include "esc.cpp"
#include "balance_display.cpp"
#include "balance_beeper.cpp"
#include "balance_leds.cpp"

otaUpdater ota;
ESC esc;
BalanceDisplay balanceDisplay;
BalanceBeeper balanceBeeper;
BalanceLEDs balanceLEDs;

void setup()
{
  // Serial.begin(115200);

  balanceDisplay.setup();
  ota.setup();
  esc.setup();
  balanceBeeper.setup();
  balanceLEDs.setup();
}

void loop()
{
  ota.loop();
  esc.loop();
  balanceDisplay.loop(esc.tempMosfet, esc.tempMotor, esc.dutyCycle, esc.voltage, esc.balanceState, esc.switchState, esc.adc1, esc.adc2);
  balanceBeeper.loop(esc.dutyCycle, esc.erpm, esc.switchState, esc.voltage);
  balanceLEDs.loop(esc.erpm, esc.switchState);
}
