// Drip
void drip() {
  static int ringPtr = 0;                                   //For each ring,
  setRing(ringPtr, ledsPerRing, colors[colorNumber]);       //Paint the color
  FastLED.show();

  if (Speed.ding()) {                                       //Time between rings
    Speed.stop();
    ringPtr++;                                              //Next ring
    if (ringPtr > ringCount) {
      ringPtr = 0;
      Hold.start();                                         //Last ring, start the hold timer
    } else {
      Speed.start();                                        //More rings, restart the speed timer.
    }
  }

  if (Hold.ding()) {
    Hold.stop();
    colorNumber++;                                          //Next color.
    if (colorNumber >= COLOR_COUNT) colorNumber = 0;
    Speed.start();                                          //Light the ings in the next color
  }
}
