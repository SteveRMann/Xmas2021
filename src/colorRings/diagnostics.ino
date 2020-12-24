//----- Diagnostics -----
void diagnostics() {
  if (diagFlag) {
    if (Diag.ding()) {      //Timeout the diagnostic mode
      Diag.stop();
      Speed.start();
      diagFlag = false;
    }

    // Turn off all LEDs
    for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
    // Turn on the selected LED
    leds[diagLED] = CRGB::Grey;
    FastLED.show();
    return;
  }
}
