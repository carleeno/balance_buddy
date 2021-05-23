#include <stdint.h>
#include <Arduino.h>
#include "config.h"

class Beeper
{
private:
  struct Beep
  {
    int startOffset = 0;
    int frequency = 0;
    bool triggered = false;
  };
  int pin;
  long beepStartTime = 0;
  bool isBeeping = false;
  int beepsLength = 0;
  struct Beep beeps[20];

  void play(Beep beep);

public:
  Beeper(int beeper_pin);

  void setup();

  void loop();

  void queueHappy();

  void queueShortSingle();

  void queueLongSingle();

  void queueSad();
};
