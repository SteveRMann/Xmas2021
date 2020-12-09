

//------------------ add_glitter() ---------------
void add_glitter()
{
  int chance_of_glitter =  10;        //percent of the time that we add glitter
  int number_of_glitters = 5;         //number of glitter sparkles to add

  int r = random8(100);
  if ( r < chance_of_glitter ) {
    for ( int j = 0; j < number_of_glitters; j++) {
      int pos = random16( NUM_LEDS);
      leds[pos] = glitterColor;        //very bright glitter
    }
  }
}


//----------------- top_glitter --------------
void top_glitter() {
  //Blink the top white at random
  static unsigned long whiteTime = 100ul;                //How long the top is white.
  unsigned int topInterval = random(10000ul);           //How long between blinks
  static int i = random8(TOP_NUM_LEDS);

  //White end?
  if (millis() - endTime > whiteTime and state == 0) {
    //Yes, set all to topColor
    for (int i = 0; i < TOP_NUM_LEDS; i++) {
      topper[i] = topColor;
    }
    FastLED.show();
    endTime = millis();
    state = 1;
  }

  //Time for another blink?
  if (millis() - endTime > topInterval and state == 1) {
    endTime = millis();
    i = random8(TOP_NUM_LEDS);                                    //Random LED
    topper[i] = glitterColor;
    FastLED.show();
    state = 0;
  }
}
