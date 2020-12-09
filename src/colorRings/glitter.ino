

//------------------ add_glitter() ---------------
void add_glitter()
{
  int chance_of_glitter =  10; // percent of the time that we add glitter
  int number_of_glitters = 5; // number of glitter sparkles to add

  int r = random8(100);
  if ( r < chance_of_glitter ) {
    for ( int j = 0; j < number_of_glitters; j++) {
      int pos = random16( NUM_LEDS);
      leds[pos] = CRGB::White; // very bright glitter
      //[0] = CRGB::White;   // Blink the topper
    }
  }
}


//----------------- top_glitter --------------
void top_glitter() {
  //Blink the topper LEDs at random

  //Time for another blink?
  if (millis() - endTime > topInterval && state == top_BlinkOff) {
    endTime = millis();
    int i = random8(TOP_NUM_LEDS);                                    //Random LED
    topper[i] = glitterColor;
    FastLED.show();
    state = top_BlinkOn;
  }

  //Blink end?
  if (millis() - endTime > blinkTime && state == top_BlinkOn) {
    //Yes, set all to topColor
    for (int i = 0; i < TOP_NUM_LEDS; i++) {
      topper[i] = topColor;
    }
    FastLED.show();
    endTime = millis();
    topInterval = random(500ul, 3000ul);    //Get a new interval between blinks.

    state = top_BlinkOff;
  }

}
