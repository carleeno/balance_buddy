#include <Arduino.h>
#include "ota_updater.h"
#include "esc.h"
#include "balance_display.h"
#include "balance_beeper.h"
#include "balance_leds.h"

ESC esc;
BalanceDisplay balanceDisplay(esc);
BalanceBeeper balanceBeeper;
BalanceLEDs balanceLEDs;
otaUpdater ota(balanceDisplay);

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
  balanceDisplay.loop();
  balanceBeeper.loop(esc.dutyCycle, esc.erpm, esc.switchState, esc.voltage);
  balanceLEDs.loop(esc.erpm, esc.switchState);
}
