#define FASTLED_INTERNAL
#include "FastLED.h"
#include "config.h"

class BalanceLEDs
{
private:
  bool directionIsForward;
  int targetBrightness;

  CRGB forward[NUM_LEDS_FORWARD];
  CRGB backward[NUM_LEDS_BACKWARD];

  // Helper function that blends one uint8_t toward another by a given amount
  void nblendU8TowardU8(uint8_t &cur, const uint8_t target, uint8_t amount);

  // Blend one CRGB color toward another CRGB color by a given amount.
  // Blending is linear, and done in the RGB color space.
  // This function modifies 'cur' in place.
  CRGB fadeTowardColor(CRGB &cur, const CRGB &target, uint8_t amount);

  // Fade an entire array of CRGBs toward a given background color by a given amount
  // This function modifies the pixel array in place.
  void fadeTowardColor(CRGB *L, uint16_t N, const CRGB &bgColor, uint8_t fadeAmount);

public:
  void setup();

  void loop(double erpm, uint16_t switchState);
};
