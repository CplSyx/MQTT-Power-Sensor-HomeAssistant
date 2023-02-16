/*
    Title:        MQTT Power Monitor w/ webconfig and HomeAssistant compatibility

    Date:         16th February 2023

    Description:  MQTT Power Monitor, based on EmonLib.
                  Forked from https://github.com/Mottramlabs/MQTT-Power-Sensor
                  Significant rework with a web config based on WiFiManager to enable non-code changes to configuration
                  Utilisation of ESP8266 "preferences" to save configuration between power cycles
                  MQTT payload now JSON formatted so can be used with HomeAssistant or similar applications

    Device:       Wemos D1 Mini (ESP8266) with MottramLabs' "Wemos D1 Mini Version ESP8266 Mains Power Sensor", connected to a YHDC SCT-013-000 Current Transformer
*/


#include <ESP8266WiFi.h>              // needed for EPS8266
#define ActivityLED 2                 // define the pinout for the activity LED
#define MODE 1                        // operating mode for the code base - "1" outputs to Serial Monitor. "0" runs silently.

/*  
    Localised print function that requires the MODE flag to be set to 1 in order to print. 
    Means we can skip Serial output if we're running in "production" (i.e. not connected to a monitor)
*/
void print(String s, bool newLine = false)
{
  if(MODE == 1)
    Serial.print(s);
    if(newLine)
      Serial.print("\r\n");
}
/* 
    Localised println function
    Calls localised print function with a flag for a newline
*/
void println(String s)
{
  print(s, true);
}

/*
    WiFi connectivity
*/
#include <WiFiClient.h>               // WiFi client
  WiFiClient espClient;               // Client connection to enable MQTT communication         
#include "WiFiManager.h"              // https://github.com/tzapu/WiFiManager/releases/tag/v2.0.15-rc.1

/*
    Webserver
*/
#define WEBSERVER_H                   // Resolve conflicts between WifiManager and ESPAsyncWebServer https://github.com/me-no-dev/ESPAsyncWebServer/issues/418#issuecomment-667976368
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "WebPages.h"                 // HTML for web server
  AsyncWebServer  server(80);         // Create AsyncWebServer object on port 80
  String header;                      // Variable to store the HTTP request
  String outputMessage;               // Variable to pass messages between HTML pages (via processor)   

/*
    Preferences
*/
#include <Preferences.h>              
  Preferences preferences;            // Initiate preferences and the variables we need for them below
  String wifiSSID;                    
  String wifiPassword;                
  String mqttURL;                     
  uint16_t mqttPort;                  
  String mqttUsername;                
  String mqttPassword;                
  String mqttTopic;                   
  double calibration;                 // Calibration adjustment to obtain correct current value from sensor. Needs to be checked against a measured current source such as a heater.

/*
    MQTT connection and functionality
*/
#include "PubSubClient.h"             // http://pubsubclient.knolleary.net/api.html
  PubSubClient pubsubClient(espClient); // ESP pubsub client
  long messageCount = 0;
  long lastMessageSent = 0;           // Time last message was sent
  static int updateInterval = 1000;   // Interval between sending updates in milliseconds
#include "MQTT_Functions.h"           // MQTT Functions (requires PubSubClient so must be included after)

/*
    EmonLibrary
*/
#include "EmonLib_CurrentOnly.h"      // Include Emon Library
EnergyMonitor emon1;                  // Create an instance
double value = 0;                     // Holds the latest Irms value


/* 
    Generates the payload we will send over MQTT
    Payload is JSON formatted for compatibility with external services such as HomeAssistant
*/
String generateMQTTPayload()  {
  
  String payload = String("");
  payload = payload + "{\"Value\":" + "\"" + String(value) + "\"" + ", ";
  payload = payload + "\"MAC Address\":" + "\"" + WiFi.macAddress() + "\"" + ", " + "\"SSID\":" + "\"" + WiFi.SSID() + "\"" + ", " + "\"IP\":" + "\"" + WiFi.localIP().toString() + "\"" + ", " + "\"count\":" + "\"" + String(messageCount)+"\"}";
  //NOTE: The above first creates an empty string via the String() operator as we can add strings to Strings, but not strings to strings. (Note capitalisation). 
  //https://arduino.stackexchange.com/questions/25125/why-do-i-get-invalid-operands-of-types-const-char-error

  return payload;

}

/* 
    Connect to Wifi using Preferences and WifiManager 
    Device will initially try to connect using a saved SSID. 
    If that is not possible (no SSID saved, or incorrect information entered) device will launch WifiManager AP
*/
void connectToWifi(String hostName, int timeout = 10)
{
  // Define if this is the "first" connection of the device
  bool firstConnection = true;

  if (preferences.isKey("wifiSSID")) // Will return false if we have never used the device before, or if we removed the key due to a failed connection
  {
    print("Connecting to WiFi with saved credentials: SSID ");
    print(wifiSSID);
    print(" Password ");
    println(wifiPassword);
    WiFi.begin(wifiSSID, wifiPassword);
    while(WiFi.status() == WL_DISCONNECTED) // Wait while the device tries to connect to Wifi. Flash the LED to provide a visual status to an observer.
    {
      digitalWrite(ActivityLED, HIGH);
      delay(50);
      digitalWrite(ActivityLED, LOW);
      delay(50);
    }
    firstConnection = false; // We've connected using a saved SSID, so clearly not the first connection we've had
  }
  else // We didn't have a saved SSID so we need to obtain one - the only way to do so is using the WifiManager AP
  {
    println("Setting up WiFi access point");
    WiFiManager wifiManager;
    wifiManager.setConnectTimeout(timeout);
    if(wifiManager.autoConnect(hostName.c_str())) // Blocking and handles the presentation of the credentials page - returns true if a successfull connection is made
    {
      // If we successfully connected, save the SSID and password to preferences.
      preferences.putString("wifiSSID", WiFi.SSID());
      preferences.putString("wifiPassword", WiFi.psk());
    }
  }

  // If we either failed to connect, or simply aren't connected, erase the SSID and password from preferences, erase the WiFi config, and restart the device.
  if(WiFi.status() == WL_CONNECT_FAILED || !WiFi.isConnected()) 
  {
      println("Failed to connect.");
      // Following should erase settings and allow AP to restart
      preferences.remove("wifiSSID");
      preferences.remove("wifiPassword");
      WiFi.disconnect(true);
      ESP.eraseConfig();
      ESP.reset();
  } 
  else
  {
    println("Connected to Wifi.");
    if(firstConnection) // Restart if this is the first time we've connected, to ensure we've saved the credentials properly
    {
      delay(2000);
      ESP.restart();
    }
  }
}

/* 
    Processor to replace "placeholder" text (indicated like %THIS%) in HTML with variables or other output. 
*/
String processor(const String& var){
  println(var);
  if(var == "WIFISSID"){
    return wifiSSID;
  }
  if(var == "WIFIPASSWORD"){
    return wifiPassword;
  }
  if(var == "MQTTURL"){
    return mqttURL;
  }
  if(var == "MQTTPORT"){
    return String(mqttPort);
  }
  if(var == "MQTTUSERNAME"){
    return mqttUsername;
  }
  if(var == "MQTTPASSWORD"){
    return mqttPassword;
  }
  if(var == "MQTTTOPIC"){
    return mqttTopic;
  }  
  if(var == "CALIBRATION"){
    return String(calibration);
  }
  if(var == "OUTPUTMESSAGE"){
    return outputMessage;
  }
  return String();
}

/* 
    Loads values saved on the ESP as preferences into our variables 
    Not strictly necessary we could just access them with the "get" methods at each location, but keeps things neat
*/
void loadPreferences()
{
  // Second parameter below are the default values for each setting if none are set
  wifiSSID = preferences.getString("wifiSSID", "none");
  wifiPassword = preferences.getString("wifiPassword", "none");
  mqttURL = preferences.getString("mqttURL", "http://127.0.0.1");
  mqttPort = preferences.getShort("mqttPort", 1883);
  mqttUsername = preferences.getString("mqttUsername", "username");
  mqttPassword = preferences.getString("mqttPassword", "none");
  mqttTopic = preferences.getString("mqttTopic", "publishtopic");
  calibration = preferences.getDouble("calibration", 87.5);
}

/* 
    Saves values we have obtained into ESP preferences 
*/
void savePreferences()
{
  preferences.putString("wifiSSID", wifiSSID);
  preferences.putString("wifiPassword", wifiPassword);
  preferences.putString("mqttURL", mqttURL);
  preferences.putUShort("mqttPort", mqttPort);
  preferences.putString("mqttUsername", mqttUsername);
  preferences.putString("mqttPassword", mqttPassword);
  preferences.putString("mqttTopic", mqttTopic);
  preferences.putDouble("calibration", calibration);
}

/*
    Setup function:
    Initialise the serial connection
    Load preferences
    Connect to WiFi
    Connect to MQTT
    Setup routes for webserver page handling
    Set calibration for ADC read
*/

void setup() {
  Serial.begin(115200);

  // Configure preferences namespace. "false" indicates NOT read-only
  preferences.begin("PowerMonitor", false); 
  loadPreferences(); // Load in saved preferences

  // I/O
  pinMode(ActivityLED, OUTPUT);
  digitalWrite(ActivityLED, LOW);

  // WiFi settings
  String newHostname = "PowerMonitor_";
  newHostname += ESP.getChipId();
  WiFi.hostname(newHostname.c_str());
  connectToWifi(newHostname);

  /* Define routes for webserver page handling and start the webserver */

  // Webserver: Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Webserver Route for handling incoming new settings <ESP_IP>/update?wifiSSID=<inputMessage1>&wifiPassword=<inputMessage2>&mqttURL=<inputMessage3>&mqttPort=<inputMessage4>&...etc
  server.on("/update", HTTP_POST, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("wifiSSID", true) && request->hasParam("wifiPassword", true) && request->hasParam("mqttURL", true) && request->hasParam("mqttPort", true) && request->hasParam("mqttUsername", true) && request->hasParam("mqttPassword", true) && request->hasParam("newCalibration", true)) 
    {
      wifiSSID = request->getParam("wifiSSID", true)->value();
      wifiPassword = request->getParam("wifiPassword", true)->value();
      mqttURL = request->getParam("mqttURL", true)->value();
      mqttPort = request->getParam("mqttPort", true)->value().toInt();
      mqttUsername = request->getParam("mqttUsername", true)->value();
      mqttPassword = request->getParam("mqttPassword", true)->value();
      mqttTopic = request->getParam("mqttTopic", true)->value();
      calibration = request->getParam("newCalibration", true)->value().toDouble();

      savePreferences();

      outputMessage = String("Output: ") + " " + wifiSSID + " " + wifiPassword + " " + mqttURL + " " + mqttPort + " " + mqttUsername + " " + mqttPassword + " " + mqttTopic + " " + calibration + " " + String("OK. Please restart device.");
      
    }
    else 
    {
      outputMessage = "Please complete all fields.";
    }    
    request->send_P(200, "text/html", update_html, processor);
  });

  // Webserber: Handle when restart button pressed on index_html
  server.on("/restart", HTTP_POST, [] (AsyncWebServerRequest *request) {
    // Reset on Confirmation page - incoming post value via Javascript submit
    if(request->hasParam("restart", true) && request->getParam("restart", true)->value() == "true") 
    {
      println("Restart Request Received");
      ESP.restart();
    }
    else // Just Show the confirmation page
    {
      request->send(200, "text/html", restart_html);
    }
  });

  // Start the webserver
  server.begin();

  /* Connect to the MQTT broker */
  IPAddress mqttAddress;
  mqttAddress.fromString(mqttURL);                  // Generate "IPAddress" object from user-provided IP address
  pubsubClient.setServer(mqttAddress, mqttPort);    // Set server details for connection
  //pubsubClient.setCallback(callback);             // Can be uncommented to handle incoming messages, but as this is a "one way" sensor it's not needed.

  /* Set the calibration value of the sensor */
  emon1.current(calibration); 
  
  //need to discard first few messages from the sensor as they are inaccurate due to initialisation
  for (int i = 0; i<20; i++)
  {
    emon1.calcIrms(1060); // The sketch reads approximately 106 samples of current in each cycle of mains at 50 Hz. 1480 samples therefore works out at 14 cycles of mains. 1060 is 10 cycles.
    delay(10);
  }

  println("**********END SETUP************");

} // SETUP END

/* 
    Loop
    Check if the MQTT client is connected. If not, reconnect.
    Every "interval" (which is 1 second, 1000ms) read the current value and send over MQTT
    Blink the LED whilst we do this to provide visual feedback to an observer
 */
void loop() {
  
  if (!pubsubClient.connected()) 
  {
    reconnect();
  } // end if
  pubsubClient.loop();

  // Once interval has passed - this means we aren't constantly reading from the ADC which causes issues with WiFi connection
  if (millis() - lastMessageSent > updateInterval) {
    digitalWrite(ActivityLED, HIGH);

    // read A/D values and store in value, averaged over 5 readings
    value = 0;
    for (int j = 0; j<5; j++)
    {
      value = value + emon1.calcIrms(1480);
      yield();  // Allow the ESP to do it's "background stuff". Important as this involves resetting the software WDT timeout!
    } 
    value = value / 5;
    
    //value = emon1.calcIrms(1480); //The sketch reads approximately 106 samples of current in each cycle of mains at 50 Hz. 1480 samples therefore works out at 14 cycles of mains.

    // get a report make and make as an array
    String payload = generateMQTTPayload();
    char payloadArray[(payload.length() + 1)];
    payload.toCharArray(payloadArray, (payload.length() + 1));

    // send a status report
    pubsubClient.publish(mqttTopic.c_str(), payloadArray);

    //Output to Serial Monitor what we have sent
    print("Published To topic: "); print(mqttTopic); print(" - Payload Sent: "); println(payloadArray);

    digitalWrite(ActivityLED, LOW);

    messageCount++;
    lastMessageSent = millis();
  }
  
} // LOOP END
