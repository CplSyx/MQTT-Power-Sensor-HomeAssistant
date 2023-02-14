/*
    Title:        MQTT 1 Way Mains Power Sensor Demo
    Date:         4th September 2021
    Version:      1
    Description:  Sample code
    Device:       ESP8266
    WiFi:		  See "WiFi_Settings.h" for wifi AP and password
    MQTT:		  See "MQTT_Settings.h" for MQTT broker and topic
*/

#include <ESP8266WiFi.h>              // needed for EPS8266
#include <WiFiClient.h>               // WiFi client
#include "WiFiManager.h"              // https://github.com/tzapu/WiFiManager/releases/tag/v2.0.15-rc.1

#define WEBSERVER_H                   // Resolve conflicts between WifiManager and ESPAsyncWebServer https://github.com/me-no-dev/ESPAsyncWebServer/issues/418#issuecomment-667976368
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Preferences.h>              // Save MQTT string
  Preferences preferences;            // Initiate preferences and the variables we need for them
  String wifiSSID;
  String wifiPassword;
  String mqttURL;
  uint16_t mqttPort;
  String mqttUsername;
  String mqttPassword;
  String mqttTopic;
  double calibration;                 //Calibration adjustment to obtain correct current value from sensor. Needs to be checked against a measured current source such as a heater.

// custom settings files
#include "WebPages.h"                 // HTML for web server
//#include "Secrets.h"                // Usernames and passwords
//#include "Wifi_Settings.h"            // custom Wifi settings
//#include "MQTT_Settings.h"            // MQTT broker and topic
#include "Project_Settings.h"         // board specific details.

// incude WiFi and MQTT functions
WiFiClient espClient;                 // for ESP8266 boards
#include "PubSubClient.h"             // http://pubsubclient.knolleary.net/api.html
PubSubClient pubsubClient(espClient); // ESP pubsub client

#include "WiFi_Functions.h"           // read wifi data
#include "MQTT_Functions.h"           // MQTT Functions

// EmonLibrary
#include "EmonLib_CurrentOnly.h"       // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

int loopCount = 0;



/*** web server related variables START ***/

  // Create AsyncWebServer object on port 80
  AsyncWebServer  server(80);

  // Variable to store the HTTP request
  String header;

  // Variable to pass messages between HTML pages (via processor)
  String outputMessage;

  // Current time
  unsigned long currentTime = millis();
  // Previous time
  unsigned long previousTime = 0; 
  // Define timeout time in milliseconds (example: 2000ms = 2s)
  const long timeoutTime = 2000;

/*** web server related variables END ***/

void setup() {
  Serial.begin(115200);

#ifdef Watchdog_ON
  // watchdog items, comment out if not used
  secondTick.attach(1, ISRwatchdog);
#endif

  // Configure preferences namespace. "false" indicates NOT read-only
  preferences.begin("PowerMonitor", false); 
  loadPreferences(); // Load in saved preferences

  // I/O
  pinMode(Network_LED, OUTPUT);
  digitalWrite(Network_LED, LOW);

  // WiFi settings
  String newHostname = "PowerMonitor_";
  newHostname += ESP.getChipId();
  WiFi.hostname(newHostname.c_str());

  bool firstConnection = true;
  if (WiFi.SSID().length() > 0)
  {
    firstConnection = false;
  }

  WiFiManager wifiManager;
  bool res;
  wifiManager.setConnectTimeout(30);
  res = wifiManager.autoConnect(newHostname.c_str());

  if(!res) 
  {
      Serial.println("Failed to connect.");
      // Following should erase settings and allow AP to restart
      WiFi.disconnect(true);
      delay(2000);
      ESP.reset();
  } 
  else
  {
    Serial.println("Connected to Wifi.");
    WiFi.setSleep(false);
    if(firstConnection)
    {
      delay(2000);
      ESP.restart();
    }
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_POST, [] (AsyncWebServerRequest *request) {

    // GET input1 value on <ESP_IP>/update?wifiSSID=<inputMessage1>&wifiPassword=<inputMessage2>&mqttURL=<inputMessage3>&mqttPort=<inputMessage4>&mqttUsername=<inputMessage5>&mqttPassword=<inputMessage6>&calibration=<inputMessage7>
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
    request->send_P(200, "text/html", index_html, processor);
  });

  // Restart button pressed on index_html
  server.on("/restart", HTTP_POST, [] (AsyncWebServerRequest *request) {
    if(request->hasParam("restart", true) && request->getParam("restart", true)->value() == "true") 
    {
      Serial.println("Restart Request Received");
      ESP.restart();
    }
    else
    {
      request->send(200, "text/html", restart_html);
    }
  });

  // Start the webserver
  server.begin();

  // connect to the MQTT broker
  IPAddress mqttAddress;
  mqttAddress.fromString(mqttURL);
  pubsubClient.setServer(mqttAddress, mqttPort);
  pubsubClient.setCallback(callback);

  emon1.current(calibration); 
  Serial.println("**********************");

  //need to discard first few messages from the sensor as they are wildly inaccurate
  for (int i = 0; i<20; i++)
  {
    emon1.calcIrms(1060); //The sketch reads approximately 106 samples of current in each cycle of mains at 50 Hz. 1480 samples therefore works out at 14 cycles of mains. 1060 is 10 cycles.
    delay(10);
  }

  // reset heartbeat timer
  LastMsg = millis();

} // end of setup

void loop() {
  
  if (!pubsubClient.connected()) 
  {
    reconnect();
  } // end if
  pubsubClient.loop();

/* Doing this blocks the loop and so causes issues with the webserver. 
  // read A/D values and store in value, averaged over 10 readings
  Value = 0;
  for (int j = 0; j<10; j++)
  {
    Value = Value + emon1.calcIrms(1480);
  } 
  Value = Value / 10;
*/

  emon1.calcIrms(1060); //The sketch reads approximately 106 samples of current in each cycle of mains at 50 Hz. 1480 samples therefore works out at 14 cycles of mains. 1060 is 10 cycles.
  // headbeat or report requested
  if (millis() - LastMsg > Heatbeat || Report_Request == 1) {

    LastMsg = millis();
    Report_Request = 0;

    // update event progress counter
    ++Heart_Value;
    if (Heart_Value > Heartbeat_Range) {
      Heart_Value = 1;
    } // end if

    // heartbeat timed out or report message requested

    // get a report make and make as an array
    String Report = Status_Report();
    char Report_array[(Report.length() + 1)];
    Report.toCharArray(Report_array, (Report.length() + 1));

    digitalWrite(Network_LED, HIGH);
    // send a status report
    pubsubClient.publish(mqttTopic.c_str(), Report_array);

    //Output to Serial Monitor what we have sent
    Serial.print(String(loopCount)); Serial.print(": Published To topic: "); Serial.print(mqttTopic); Serial.print(" - Report Sent: "); Serial.println(Report_array);

    // only used to make the LED flash visable
    //delay(10);

    digitalWrite(Network_LED, LOW);

  } // end of heartbeat timer
  
  loopCount++; //increment counter
  
  

} // end of loop

// Processor to replace "placeholder" text (indicated like %THIS%) in HTML with variables or other output.
String processor(const String& var){
  Serial.println(var);
  if(var == "WIFISSID"){
    return WiFi.SSID();
  }
  if(var == "WIFIPASSWORD"){
    return WiFi.psk();
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
