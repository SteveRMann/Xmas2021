#define SKETCH "TwinkleFox.ino"
#define VERSION "2.00"           // Four characters

/*
   TwinkleFOX: Twinkling 'holiday' lights that fade in and out.
   Colors are chosen from a palette; a few palettes are provided.
   Features:
   - smoother fading, compatible with any colors and any palettes
   - easier control of twinkle speed and twinkle density
   - supports an optional 'background color'
   - takes even less RAM: zero RAM overhead per pixel
   - illustrates a couple of interesting techniques (uh oh...)

    The idea behind this (new) implementation is that there's one
    basic, repeating pattern that each pixel follows like a waveform:
    The brightness rises from 0..255 and then falls back down to 0.
    The brightness at any given point in time can be determined as
    as a function of time, for example:
    brightness = sine( time ); // a sine wave of brightness over time

    So the way this implementation works is that every pixel follows
    the exact same wave function over time.  In this particular case,
    I chose a sawtooth triangle wave (triwave8) rather than a sine wave,
    but the idea is the same: brightness = triwave8( time ).

    Of course, if all the pixels used the exact same wave form, and
    if they all used the exact same 'clock' for their 'time base', all
    the pixels would brighten and dim at once -- which does not look
    like twinkling at all.

    So to achieve random-looking twinkling, each pixel is given a
    slightly different 'clock' signal.  Some of the clocks run faster,
    some run slower, and each 'clock' also has a random offset from zero.
    The net result is that the 'clocks' for all the pixels are always out
    of sync from each other, producing a nice random distribution
    of twinkles.

    The 'clock speed adjustment' and 'time offset' for each pixel
    are generated randomly.  One (normal) approach to implementing that
    would be to randomly generate the clock parameters for each pixel
    at startup, and store them in some arrays.  However, that consumes
    a great deal of precious RAM, and it turns out to be totally
    unnessary!  If the random number generate is 'seeded' with the
    same starting value every time, it will generate the same sequence
    of values every time.  So the clock adjustment parameters for each
    pixel are 'stored' in a pseudo-random number generator!  The PRNG
    is reset, and then the first numbers out of it are the clock
    adjustment parameters for the first pixel, the second numbers out
    of it are the parameters for the second pixel, and so on.
    In this way, we can 'store' a stable sequence of thousands of
    random clock adjustment parameters in literally two bytes of RAM.

    There's a little bit of fixed-point math involved in applying the
    clock speed adjustments, which are expressed in eighths.  Each pixel's
    clock speed ranges from 8/8ths of the system clock (i.e. 1x) to
    23/8ths of the system clock (i.e. nearly 3x).

    On a basic Arduino Uno or Leonardo, this code can twinkle 300+ pixels
    smoothly at over 50 updates per seond.

    -Mark Kriegsman, December 2015

*/

//#include <ALib0.h>

//---------- wifi ----------
#define HOSTPREFIX "XMAS-"   //18 chars max
#include "ESP8266WiFi.h"    //Not needed if also using the Arduino OTA Library...
#include <Kaywinnet.h>
char macBuffer[24];         //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];          //Holds hostNamePrefix + the last three bytes of the MAC address.


// ---------- ota ----------
#include <ArduinoOTA.h>


// --------------- FastLED ---------------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>
#include <myXmasOutsideLeds.h>
CRGBArray<NUM_LEDS> leds;         //Original line
//CRGB leds[NUM_LEDS];



// Overall twinkle speed.
// 0 (VERY slow) to 8 (VERY fast).
// 4-6 are recommended, default is 4.
#define TWINKLE_SPEED 4

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once). Default is 5.
#define TWINKLE_DENSITY 4

// How often to change color palettes.
#define SECONDS_PER_PALETTE  20
// Also: toward the bottom of the file is an array
// called "ActivePaletteList" which controls which color
// palettes are used; you can add or remove color palettes
// from there freely.

// Background color for 'unlit' pixels
CRGB gBackgroundColor = CRGB::Black;

// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 1


CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;

// --------------- setup ---------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

  setup_wifi();
  start_OTA();

  delay(100);   //safety startup delay
  //FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
  .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  chooseNextColorPalette(gTargetPalette);
  
}



// --------------- loop ---------------
void loop() {
  ArduinoOTA.handle();

  EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {
    chooseNextColorPalette( gTargetPalette );
  }

  EVERY_N_MILLISECONDS( 10 ) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 12);
  }

  drawTwinkles( leds);                    //Twinkles all LEDS

  topper(NUM_LEDS - NUM_TOP, NUM_LEDS);   //Handle the topper

  FastLED.show();
}
