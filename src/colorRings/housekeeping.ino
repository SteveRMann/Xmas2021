// - - - Housekeeping stuff to do with every loop  - - -
void housekeeping() {
  //ESP.wdtFeed();

  ArduinoOTA.handle();

  /*
  //Make sure we stay connected to the mqtt broker
  if (!client.connected()) {
    mqttConnect();
  }
  client.loop();                      //Check for MQTT messages
  */
}
