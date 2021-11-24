//
//  "Pacifica"
//  Gentle, blue-green ocean waves.
//  December 2019, Mark Kriegsman and Mary Corey March.
//  For Dan.
//

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#include "myLeds.h"
#undef MAX_MA
#define MAX_MA 500
CRGB leds[NUM_LEDS];


// ------------ setup() ------------
void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
  .setCorrection( TypicalLEDStrip );
  //FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
}


// ------------ loop() ------------
void loop() {
  EVERY_N_MILLISECONDS( 20) {
    pacifica_loop();
    topper(NUM_LEDS - NUM_TOP, NUM_LEDS);   //Handle the topper
    black(1, 41);                           //Make unused LEDS black
    FastLED.show();
  }
}
