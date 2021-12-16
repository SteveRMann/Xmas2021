#define SKETCH "countLeds.ino"
#define FASTLED_INTERNAL        // Pragma fix
#include "FastLED.h"

// countLeds
// This sketch lights up all LEDs, but with every tenth LED green and
// every hundredth is yellow.
//

#include "myTreeLeds.h"
CRGB leds[NUM_LEDS];


void setup() {
  int j;
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  fill_solid( leds, NUM_LEDS, CRGB::Black);           //All leds off
  FastLED.show();
  FastLED.show();
  FastLED.delay(1000);

  lightEmUp();
}



// ---------- loop() ----------
void loop() {
}



// Function to light up all LEDs as gray, but with every 
// tenth LED green and every hundredth is yellow.
void lightEmUp() {
  fill_solid(leds, NUM_LEDS, CRGB::DimGrey);
  for (int i = 0; i < NUM_LEDS; i++) {
    int j = i % 10;                     //j==0 every ten steps
    if (j == 0) {
      leds[i] = CRGB::Green;
    }
    j = i % 100;                        //j==0 every hundred steps
    if (j == 0) {
      leds[i] = CRGB::Yellow;
    }
  }
  FastLED.show();
}
