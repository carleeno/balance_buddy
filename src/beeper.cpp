#include "beeper.h"

Beeper::Beeper(int beeper_pin)
{
  pin = beeper_pin;
}

void Beeper::setup()
{
  if (FREQ_CONTROL)
  {
    ledcSetup(0, 2000, 8); // setup beeper
    ledcAttachPin(pin, 0); // attach beeper
  }
  else
  {
    pinMode(BEEPER_PIN, OUTPUT);
    digitalWrite(BEEPER_PIN, 0);
  }
}

void Beeper::loop()
{
  if (isBeeping)
  {
    long timeInBeeping = millis() - beepStartTime;
    for (int i = 0; i < beepsLength; i++)
    {
      if (timeInBeeping > beeps[i].startOffset && beeps[i].triggered == false)
      {
        beeps[i].triggered = true;
        play(beeps[i]);

        if (i == beepsLength - 1)
        {
          isBeeping = false;
        }
      }
    }
  }
}

void Beeper::play(Beeper::Beep beep)
{
  if (FREQ_CONTROL)
    ledcWriteTone(0, beep.frequency); // play tone
  else if (beep.frequency > 0)
    digitalWrite(BEEPER_PIN, 1); // pure beep
  else
    digitalWrite(BEEPER_PIN, 0); // beep off
}

void Beeper::queueHappy()
{
  if (isBeeping)
  {
    return;
  }

  beepStartTime = millis();
  isBeeping = true;
  beepsLength = 9;
  // On 1
  beeps[0].startOffset = 0;
  beeps[0].frequency = 2000;
  beeps[0].triggered = false;
  // Off
  beeps[1].startOffset = 270;
  beeps[1].frequency = 0;
  beeps[1].triggered = false;
  // On 2
  beeps[2].startOffset = 300;
  beeps[2].frequency = 2300;
  beeps[2].triggered = false;
  // Off
  beeps[3].startOffset = 370;
  beeps[3].frequency = 0;
  beeps[3].triggered = false;
  // On 3
  beeps[4].startOffset = 400;
  beeps[4].frequency = 2600;
  beeps[4].triggered = false;
  // Off
  beeps[5].startOffset = 570;
  beeps[5].frequency = 0;
  beeps[5].triggered = false;
  // On 4
  beeps[6].startOffset = 600;
  beeps[6].frequency = 2900;
  beeps[6].triggered = false;
  // Off
  beeps[7].startOffset = 900;
  beeps[7].frequency = 0;
  beeps[7].triggered = false;
  // Finish off
  beeps[8].startOffset = 1200;
  beeps[8].frequency = 0;
  beeps[8].triggered = false;
}

void Beeper::queueShortSingle()
{
  if (isBeeping)
  {
    return;
  }

  beepStartTime = millis();
  isBeeping = true;
  beepsLength = 3;
  // On 1
  beeps[0].startOffset = 0;
  beeps[0].frequency = 2600;
  beeps[0].triggered = false;
  // Off
  beeps[1].startOffset = 250;
  beeps[1].frequency = 0;
  beeps[1].triggered = false;
  // Finish off
  beeps[2].startOffset = 500;
  beeps[2].frequency = 0;
  beeps[2].triggered = false;
}

void Beeper::queueLongSingle()
{
  if (isBeeping)
  {
    return;
  }

  beepStartTime = millis();
  isBeeping = true;
  beepsLength = 3;
  // On 1
  beeps[0].startOffset = 0;
  beeps[0].frequency = 2600;
  beeps[0].triggered = false;
  // Off
  beeps[1].startOffset = 750;
  beeps[1].frequency = 0;
  beeps[1].triggered = false;
  // Finish off
  beeps[2].startOffset = 1000;
  beeps[2].frequency = 0;
  beeps[2].triggered = false;
}

void Beeper::queueSad()
{
  if (isBeeping)
  {
    return;
  }

  beepStartTime = millis();
  isBeeping = true;
  beepsLength = 9;
  // Sad
  beeps[0].startOffset = 0;
  beeps[0].frequency = 2900;
  beeps[0].triggered = false;
  // Off
  beeps[1].startOffset = 100;
  beeps[1].frequency = 0;
  beeps[1].triggered = false;
  // Sad
  beeps[2].startOffset = 200;
  beeps[2].frequency = 2600;
  beeps[2].triggered = false;
  // Off
  beeps[3].startOffset = 300;
  beeps[3].frequency = 0;
  beeps[3].triggered = false;
  // Sad
  beeps[4].startOffset = 400;
  beeps[4].frequency = 2300;
  beeps[4].triggered = false;
  // Off
  beeps[5].startOffset = 500;
  beeps[5].frequency = 0;
  beeps[5].triggered = false;
  // Sad
  beeps[6].startOffset = 600;
  beeps[6].frequency = 2000;
  beeps[6].triggered = false;
  // Off
  beeps[7].startOffset = 1200;
  beeps[7].frequency = 0;
  beeps[7].triggered = false;
  // Finish off
  beeps[8].startOffset = 1500;
  beeps[8].frequency = 0;
  beeps[8].triggered = false;
}
