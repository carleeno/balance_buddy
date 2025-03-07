#include "balance_beeper.h"

BalanceBeeper::BalanceBeeper() : beeper(BEEPER_PIN) {}

void BalanceBeeper::setup()
{
  beeper.setup();
  if (PLAY_STARTUP)
  {
    beeper.queueHappy();
  }
}

void BalanceBeeper::loop(double dutyCycle, double erpm, uint16_t switchState, double voltage)
{
  beeper.loop();

  // Non latching beeps for Duty Cycle
  if (fabsf(dutyCycle) > DUTY_CYCLE_ALERT && DUTY_CYCLE_ALERT > 0)
  {
    beeper.queueShortSingle();
  }

  // Latching beep for HALF footpad state
  if (switchState == 1 && fabsf(erpm) > SWITCH_ERPM && SWITCH_ERPM > 0 && switchStateLatch == false)
  {
    switchStateLatch = true;
    beeper.queueLongSingle();
  }
  else if (switchState != 1)
  {
    switchStateLatch = false;
  }

  // Low voltage, time based repeat
  if (voltage < LOW_VOLTAGE && LOW_VOLTAGE > 0 && lastLowVoltageMillis + LOW_VOLTAGE_INTERVAL < millis())
  {
    beeper.queueSad();
    lastLowVoltageMillis = millis();
  }
}
