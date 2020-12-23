
/* ================================== mqttConnect() =================================
  Include at the top of the main ino file:
  //---------- MQTT ----------
  #include <ESP8266WiFi.h>        //Connect (and reconnect) an ESP8266 to the a WiFi network.
  #include <PubSubClient.h>       //connect to a MQTT broker and publish/subscribe messages in topics.
  #include <Kaywinnet.h>          //Net credentials.

  //Declare an object of class WiFiClient, which allows to establish a connection to a specific IP and port
  //Declare an object of class PubSubClient, which receives as input of the constructor the previously defined WiFiClient.
  //The constructor MUST be unique on the network.
  WiFiClient colrrings;
  PubSubClient client(colrrings);

  #define NODENAME "colorRings"                               //Give this node a name
  const char *cmndTopic = NODENAME "/cmnd";                   //Incoming commands, payload is a command.
  const char *connectName =  NODENAME "1";                    //Must be unique on the network
  const char *mqttServer = MQTT_SERVER;                       //Broker credentials defined in Kaywinnet.h
  const int mqttPort = MQTT_PORT;

  //Build an array of topics to subscribe to in mqttConnect()
  static const char *mqttSubs[] = {
  cmndTopic
  };



  //----------
  //IN SETUP(), after setup_wifi().
  //Call the setServer method on the PubSubClient object, passing as first argument the
  //address and as second the port.
  client.setServer(mqttServer, mqttPort);
  mqttConnect();

  //Show the topics:
  Serial.print(F("statusTopic= "));
  Serial.println(statusTopic);



  //----------
  //IN LOOP()
    //Make sure we stay connected to the mqtt broker
    if (!client.connected()) {
      mqttConnect();
    }
    client.loop();                      //Check for MQTT messages


  //----------
  //Where you need to publish:
  client.publish(Topic, "on");

*/


void mqttConnect() {
  while (!client.connected()) {
    Serial.print(F("MQTT connection, "));
    if (client.connect(connectName)) {
      Serial.print(F("connected as "));
      Serial.println(connectName);

      client.setCallback(callback);
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F("- trying again in 5-seconds."));
      delay(5000);
    }


    //Subscriptions:
    //      client.subscribe(cmndTopic);
    //      Serial.print(F("Subscribing to "));
    //      Serial.println(cmndTopic);
    //The list of topics to subscribe to is defined in the globals at the top of the sketch.
    char subTopic[40];
    byte numberOfSubs = (sizeof(mqttSubs) / sizeof(mqttSubs[0]));
    Serial.print(numberOfSubs);
    Serial.println(F(" subscriptions: "));
    for (int i = 0 ; i < numberOfSubs; i++) {
      strcpy(subTopic, mqttSubs[i]);
      Serial.print(F("Subscribing to "));
      Serial.println(subTopic);
      client.subscribe(subTopic);
      //delay(pubsubDelay);
    }
  }
}



//==================================  mqtt callback ==================================
//This function is executed when some device publishes a message to a topic that this ESP8266 is subscribed to.
//
void callback(String topic, byte *message, unsigned int length) {

  char mess[24];

  Serial.println();
  Serial.print(F("Message arrived on topic: "));
  Serial.println(topic);
  Serial.print(F("Message length= "));
  Serial.println(length);

  for (unsigned int i = 0; i < length; i++) {
    mess[i] = message[i];
  }

  /*
    //Convert the character array to a string1
    String messageString;
    for (unsigned int i = 0; i < length; i++) {
      messageString += (char)message[i];
      mess[i] = message[i];
    }
    messageString.trim();
    messageString.toUpperCase();          //Make the string upper-case

    Serial.print("messageString: ");
    Serial.print(messageString);
    Serial.println();
  */
  toLow(mess);         //Make mess LowerCase
  Serial.print("message: ");
  Serial.print(mess);
  Serial.println();



  if (topic == cmndTopic) {
    Serial.println(F("Received cmndTopic"));

    if (!strcmp(mess, "reset")) {          //if mess=="reset", then strcmp returns a zero (false).
      diagFlag = false;
      //Serial.println(F("diagFlag=false"));
    }
  }

  if (topic == ledTopic) {
    Serial.println(F("Received ledTopic"));
    diagFlag = true;
    diagLED = atoi(mess);
    if (diagLED < 0) diagLED = 0;         //No negative LED numbers.
  }

}



void toUp(char *p) {
  while (*p) {
    *p = toupper(*p);
    p++;
  }
}

void toLow(char *p) {
  while (*p) {
    *p = tolower(*p);
    p++;
  }
}
