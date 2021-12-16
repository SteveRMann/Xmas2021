

//------------------ add_glitter() ---------------
void add_glitter()
{
  int chance_of_glitter =  10;        // percent of the time that we add glitter
  int number_of_glitters = 5;         // number of glitter sparkles to add

  int r = random8(100);
  if ( r < chance_of_glitter ) {
    for ( int j = 0; j < number_of_glitters; j++) {
      int pos = random16( NUM_LEDS);
      leds[pos] = CRGB::White;        // very bright glitter
    }
  }
}


//----------------- top_glitter --------------
void top_glitter() {
  //Blink the top white at random
  static unsigned long whiteTime = 50ul;                // How long the top is white.
  unsigned int topInterval = random(30000ul);           // How long between blinks


  if (millis() - endTime > whiteTime and state == 0) {
    top[0] = topColor;
    endTime = millis();
    state = 1;
  }

  if (millis() - endTime > topInterval and state == 1) {
    endTime = millis();
    top[0] = CRGB::White;
    state = 0;
  }
}
