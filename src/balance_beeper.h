#include "beeper.h"
#include "config.h"

class BalanceBeeper
{
private:
  Beeper beeper;

  bool switchStateLatch = false;
  long lastLowVoltageMillis = 0;

public:
  BalanceBeeper();
  
  void setup();

  void loop(double dutyCycle, double erpm, uint16_t switchState, double voltage);
};
