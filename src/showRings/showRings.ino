#define SKETCH "showRings.ino"



//---------- wifi ----------
#define HOSTPREFIX "showRings"      //18 chars max
#include "ESP8266WiFi.h"            //Not needed if also using the Arduino OTA Library...
#include <Kaywinnet.h>              //WiFi credentials
char macBuffer[24];                 //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];                  //Holds hostNamePrefix + the last three bytes of the MAC address.


// ---------- ota ----------
#include <ArduinoOTA.h>


// ---------- Timers ----------
#include <dlay.h>
dlay dripTimer;
dlay sequenceTimer;
dlay sequenceDelay;



// ---------- fastLED ----------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>

#define NUM_LEDS 785
#define LED_PIN_1 D3             //Tree topper.
#define LED_PIN_2 D2             //Tree main
#define COLOR_ORDER RGB
#define BRIGHTNESS 75
#define LED_TYPE WS2811
#define TOP_TYPE WS2812

int colorPtr = 0;
const int colorCount = 4;
CRGB colors[colorCount] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};
CRGB topColor = CRGB::Blue;

CRGB top[1];                    // Tree topper, one bulb
CRGB leds[NUM_LEDS];            // Array for the string of LEDS
uint8_t data[NUM_LEDS];

int paletteNumber;
unsigned long topFlash;;
unsigned long endTime = 0;
int state = 0;

//define palettes and LED Ring Arrays
#include "palettes.h"
#include "rings.h"            //ringCount is initialized here.

int ringPtr = 0;              //Current ring number
int dripSpeed = 25;           //Smaller is faster
int dripPeriod = 6000;        //How often a drip starts
int sequenceDelayMs = 1000;   //How long between sequences.



//----------------------- setRing() -------------------
//Set a selected ring to a single color
//Enter with ring number, led count in the ring, and the color.
//Example:
//  setRing(3,sizeof(ring[3]),RED;
//  FastLED.show();

void setRing(int rn, int rlc, CRGB ringColor) {
  //Debugging
  //Serial.println();
  //Serial.print(F("Ring number: "));
  //Serial.println(rn);
  //Serial.print(F("Ring LED count= "));
  //Serial.println(rlc);

  for (int i = 0; i < rlc; i++) {
    leds[ring[rn][i]] = ringColor;
    //debug
    //Serial.println (ring[rn][i]);
  }
}




void nextRingLed(int rn, int rlc, CRGB ringColor) {
  //This function increments the ring then paints it with the argument color.
  static int i = 0;
  if (i >= rlc) {
    //done.
    i = 0;
    colorPtr++;
    if (colorPtr > 2)
      colorPtr = 0;
    return;
  }

  if (rn == 31) i++;
  Serial.print(F("rn= "));
  Serial.print(rn);
  Serial.print(F(",  i= "));
  Serial.println(i);

  leds[ring[rn][i]] = ringColor;    //Picks one LED in the ring array of LED numbers.
  FastLED.show();
  i++;
}


// ========================== fadeall ==========================
void fadeAll()  {
  //Every call to fadeall() reduces the intensity of all by about 1%
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}




void  fadeUp () {
  static int FastLED_fade_counter = 1;
  for ( int i = 0; i < NUM_LEDS; i++ )
  {
    leds[i] = CRGB::Orange; // Can be any colour
    leds[i].maximizeBrightness(FastLED_fade_counter);
  }
  FastLED.show();
  FastLED_fade_counter ++ ;   // Increment
}




//----------------------- setup() -------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

  //  setup_wifi();
  //  start_OTA();

  delay(100);
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<TOP_TYPE, LED_PIN_1, COLOR_ORDER>(top, 1).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  /*
    // Init the topper
    top[0] = CRGB::Blue;
    FastLED.show();

    paletteNumber = 5;        // Show the default palette first.
  */


  // ----- Turn off all LEDs -----
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
  FastLED.show();
  delay(1000);
  // ----------------------------


  //start with the tree all green.
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Green;
  FastLED.show();


  // start timers
  dripTimer.setTime(dripSpeed, true);              //How fast does the drip occur.
  sequenceTimer.setTime(dripPeriod, true);         //How long in ms between sequences.
  sequenceDelay.setTime(sequenceDelayMs, false);   //When to restart the whole sequence.
}



//----------------------- loop() -------------------
void loop() {
  ArduinoOTA.handle();

  // Drip
  //Every dripSpeed ms, bump the ring number and set that ring to white
  if (dripTimer.ding()) {
    setRing(ringPtr, ledsPerRing, CRGB::White);          //ringPtr inits to 0 in setup()
    FastLED.show();
    dripTimer.start();                         //Restart the timer

    ringPtr++;                                 //Bump ringPtr, if > if all rings are white, turn off the drip.
    if (ringPtr > ringCount) {
      dripTimer.stop();                        //Stop the drip timer
      ringPtr = 0;                             //Reset the ring pointer for the next drip.
    }
  }


  // Start over
  //When sequenceTimer dings, start over. Set the tree to green and enable sequenceDelay so that in one second
  //the sequence starts over.
  if (sequenceTimer.ding()) {
    sequenceTimer.stop();

    //Fade to black then fade-up to green.
    for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 2) {
      FastLED.show();
      fadeAll();
      //delay(1);
    }

    for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 2) {
      FastLED.show();
      fadeUp();
      //delay(1);
    }

    //for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Blue;
    //FastLED.show();
    sequenceDelay.setTime(sequenceDelayMs, true);      //Restart sequenceDelay. In one second, start the drip again.
  }


  //Pause sequenceDelay ms before starting the dripTimer again.
  if (sequenceDelay.ding()) {
    sequenceDelay.stop();
    dripTimer.setTime(dripSpeed, true);                //Restart dripTimer
    sequenceTimer.setTime(dripPeriod, true);           //Restart sequenceTimer
  }

}
