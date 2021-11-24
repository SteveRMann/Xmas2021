// fadeTowardColor.ino

#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>

// fadeTowardColor
//
// Fades one random RGB color toward the background RGB color
// Also includes a function for fading a whole array of pixels toward a given color
//
// Mark Kriegsman
// https://gist.github.com/kriegsman
// December 2016


#include "myLeds.h"
CRGB leds[NUM_LEDS];


// ---------- setup ----------
void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}




// ---------- loop ----------
void loop(){
  CRGB bgColor( 15, 255, 2); // pine green in RGB

  // fade all existing pixels toward bgColor by "5" (out of 255)
  fadeTowardColor( leds, NUM_LEDS, bgColor, 5);

  // periodically set random pixel to a random color, to show the fading
  EVERY_N_MILLISECONDS(50) {               //Lower = faster.
    uint16_t pos = random16( NUM_LEDS);
    CRGB color = CRGB( random8(), 0, random8());
    leds[ pos ] = color;
  }

  FastLED.show();
  FastLED.delay(20);
}
