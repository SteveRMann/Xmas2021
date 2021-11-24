#define SKETCH "ledTest.ino"

//Simple sketch to light up all LEDs in each primary color.
//Red, Green, Blue, White, Black



// ---------- fastLED ----------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>
#include "myLeds.h"

int colorPtr = 0;
const int colorCount = 5;
CRGB colors[colorCount] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White, CRGB::Black};
CRGB leds[NUM_LEDS];        // Array for the string of LEDS




//----------------------- setup() -------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));


  delay(100);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);


  // ----- Turn off all LEDs -----
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
  FastLED.show();
  delay(1000);
  // ----------------------------

}



//----------------------- loop() -------------------
void loop() {
  for (int i = 0; i < colorCount; i++) {
    for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = colors[i];
    FastLED.show();
    FastLED.delay(1000);
  }
}
