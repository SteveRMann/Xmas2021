
//------------------ next_palette() ---------------
void next_palette() {
  paletteNumber += 1;
  if (paletteNumber > 2) paletteNumber = 0;
}


//------------------ render data array with palette() ---------------
void render_data_with_palette(int paletteNum) {
  for ( int i = 0; i < NUM_LEDS; i++) {
    switch (paletteNum) {
      case 0:
        leds[i] = ColorFromPalette( rgbPalette, data[i], 128, LINEARBLEND );
        break;
      case 1:
        leds[i] = ColorFromPalette( xmasPalette, data[i], 128, LINEARBLEND );
        break;
      case 2:
        leds[i] = ColorFromPalette( gPalette, data[i], 128, LINEARBLEND );
        break;
      default:
        leds[i] = ColorFromPalette( xPalette, data[i], 128, LINEARBLEND );
        break;
    }
  }
}
