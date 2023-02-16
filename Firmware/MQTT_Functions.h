// the ESP8266's MAC address is normally used to send a message to a selected device. 
// below is the address used to broadcast to all devices subscribed to the above topic.

String Broadcast_All = "*ALL";

// test if message received
void callback(char* topic, byte* payload, unsigned int length_1) {

#ifdef Print_Report_Level_1
  Serial.print("Message length = "); Serial.println(length_1);
#endif

  // make a string of the received message
  String  Message_Arrived = "";
  for (unsigned int i = 0; i < length_1; i++) {
    Message_Arrived = Message_Arrived + ((char)payload[i]);
  }

#ifdef Print_Report_Level_1
  Serial.print("Message arrived and made into string: "); Serial.println(Message_Arrived);
#endif

  // is the message for only you? Either contains your MAC address or the for all address (broadcast)
  if (Message_Arrived.indexOf(WiFi.macAddress()) >= 0 || Message_Arrived.indexOf(Broadcast_All) >= 0) {

    // valid message received, remove the headers either All or Mac
    // remove *ALL if present
    if  (Message_Arrived.indexOf(Broadcast_All) >= 0) {
      // remove the address string characters
      Message_Arrived.replace(Broadcast_All, "");
    } // end of strip and test for ALL broadcast

    // remove the MAC number if present
    if  (Message_Arrived.indexOf(WiFi.macAddress()) >= 0) {
      // remove the address string characters
      Message_Arrived.replace(WiFi.macAddress(), "");
    } // end of strip and test for ALL broadcast

    // ************************************
    // add other commands here
    // ************************************

    // test for Reboot command
    if ((Message_Arrived.indexOf("#REBOOT") >= 0) ) {

      Serial.println("Reboot Request!");
      ESP.restart();

    } // end of reboot test function


  } // end of a valid message received

  yield(); // Ensures we're not blocking the ESP8266 functions https://stackoverflow.com/questions/34497758/what-is-the-secret-of-the-arduino-yieldfunction

} // end of callback


// connect to MQTT broker
void reconnect() {

  // loop until we're reconnected
  while (!pubsubClient.connected()) {

    // attempt to connect
    Serial.print("Attempting MQTT Broker connection...");

    // make MAC address array used for Client ID
    //char  MAC_array[My_MAC.length()];
    //My_MAC.toCharArray(MAC_array, (My_MAC.length() + 1));

    // connect client and use MAC address array as the Client ID
    if (pubsubClient.connect(WiFi.macAddress().c_str(), mqttUsername.c_str(), mqttPassword.c_str())) {

      Serial.println("connected");
      Serial.print("This is the client ID Used: "); Serial.println(WiFi.macAddress());

      // ... and resubscribe
      pubsubClient.subscribe(mqttTopic.c_str());
      delay(10);  // It needs a delay here else does not subsribe correctly!
      Serial.print("Sunbscribed to: "); Serial.println(mqttTopic.c_str());

    } // end of if connected

    else {

      Serial.print("Failed, rc=");
      Serial.print(pubsubClient.state());
      Serial.println(" Try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);

    } // end of else

  } // end of while loop

} // end of function
