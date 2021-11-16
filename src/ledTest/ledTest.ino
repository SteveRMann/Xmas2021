#define SKETCH "ledTest.ino"



//---------- wifi ----------
#define HOSTPREFIX "ledTest"    //18 chars max
#include "ESP8266WiFi.h"        //Not needed if also using the Arduino OTA Library...
#include <Kaywinnet.h>          // WiFi credentials
char macBuffer[24];             //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];              //Holds hostNamePrefix + the last three bytes of the MAC address.


// ---------- ota ----------
#include <ArduinoOTA.h>


// ---------- Timers ----------
//unsigned long startMillis1;
//unsigned long startMillis2;
//unsigned long currentMillis;
//unsigned long loopMillis;
//const unsigned long oneSecond = 1000;

#include <timeObj.h>
timeObj aTimer;
int aTimerStatus = 0;
timeObj bTimer;
int bTimerStatus = 0;
timeObj cTimer;
int cTimerStatus = 0;



// ---------- fastLED ----------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>

#define NUM_LEDS 785
#define LED_PIN_1 D2    //Tree topper.
#define LED_PIN_2 D3    //Tree main
#define COLOR_ORDER RGB
#define BRIGHTNESS 75
#define LED_TYPE WS2811
#define TOP_TYPE WS2812

int colorPtr = 0;
const int colorCount = 4;
CRGB colors[colorCount] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};
CRGB topColor = CRGB::Blue;

CRGB top[1];                // Tree topper, one bulb
CRGB leds[NUM_LEDS];        // Array for the string of LEDS
uint8_t data[NUM_LEDS];

int paletteNumber;
unsigned long topFlash;;
unsigned long endTime = 0;
int state = 0;

//define palettes and LED Ring Arrays
#include "palettes.h"
#include "rings.h"    //ringCount is initialized here.

int ringPtr = 0;
int dripSpeed = 25;       //Smaller is faster
int dripPeriod = 6000;    //How often a drip starts



//----------------------- setRing() -------------------
//Set a selected ring to a single color
//Enter with ring number, led count in the ring, and the color.
//Example:
//  setRing(3,sizeof(ring[3]),RED;
//  FastLED.show();

void setRing(int rn, int rlc, CRGB ringColor) {
  //Debugging
  Serial.println();
  Serial.print(F("Ring number: "));
  Serial.println(rn);
  Serial.print(F("Ring LED count= "));
  Serial.println(rlc);

  for (int i = 0; i < rlc; i++) {
    leds[ring[rn][i]] = ringColor;
    //debug
    Serial.println (ring[rn][i]);
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
  aTimer.setTime(dripSpeed);
  aTimer.start();
  aTimerStatus = 0;
  bTimer.setTime(dripPeriod);
  bTimer.start();
  bTimerStatus = 0;
  cTimer.setTime(10000);
  cTimer.start();
  cTimerStatus = 0;

}



//----------------------- loop() -------------------
void loop() {
  ArduinoOTA.handle();
  //currentMillis = millis();


  // Drip
  //Every second, bump the ring number and set the ring to white
  if (aTimer.ding() && aTimerStatus == 0) {
    Serial.print(F("colorPtr= "));
    Serial.println(colorPtr);
    setRing(ringPtr, 3, CRGB::White);       //ringPtr inits to 0 in setup()
    FastLED.show();

    //aTimer.start();                       //Restart the timer
    aTimer.setTime(dripSpeed);                   //Because ding means the count has counted down to zero?

    ringPtr++;                              //Bump ringPtr, if > ringCount then turn off the drip.
    if (ringPtr > ringCount) {
      ringPtr = 0;                          //Reset the ring pointer for the next drip.
      aTimerStatus = 1;                     //Turn aTimer "off"
    }
  }


  // Start over
  //When bTimer dings, start over. Set the tree to green and enable cTimer for one second.
  if (bTimer.ding() && bTimerStatus == 0) {
    bTimerStatus = 1;
    for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Green;
    FastLED.show();
    cTimer.setTime(1000);                     //Reset cTimer
    cTimerStatus = 0;                         //Enable cTimer
  }


  //Delay ends.
  //Reset the flag to let aTimer cycle again.
  if (cTimer.ding() && cTimerStatus == 0) {   //Replaces delay(1000)
    cTimerStatus = 1;                         //Stop cTimer
    aTimer.setTime(dripSpeed);                //Reset aTimer
    aTimerStatus = 0;                         //Enable aTimer to start the drip
    bTimer.setTime(dripPeriod);               //Reset bTimer
    bTimerStatus = 0;                         //Enable bTimer to start over.
  }

}
