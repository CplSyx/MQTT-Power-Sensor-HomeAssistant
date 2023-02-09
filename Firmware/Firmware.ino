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

// custom settings files
#include "Secrets.h"                  // Usernames and passwords
#include "Wifi_Settings.h"            // custom Wifi settings
#include "MQTT_Settings.h"            // MQTT broker and topic
#include "Project_Settings.h"         // board specific details.

// incude WiFi and MQTT functions
WiFiClient espClient;                 // for ESP8266 boards
#include "PubSubClient.h"             // http://pubsubclient.knolleary.net/api.html
PubSubClient pubsubClient(espClient);       // ESP pubsub client
#include "WiFi_Functions.h"           // read wifi data
#include "MQTT_Functions.h"           // MQTT Functions

// EmonLibrary
#include "EmonLib_CurrentOnly.h"        // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

int loopCount = 0;

/*** web server related variables START ***/

  // Set web server port number to 80
  WiFiServer server(80);

  // Variable to store the HTTP request
  String header;

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

  // I/O
  pinMode(Network_LED, OUTPUT);
  digitalWrite(Network_LED, LOW);

  String newHostname = "PowerMonitor_";
  newHostname += ESP.getChipId();
  WiFi.hostname(newHostname.c_str());

  WiFiManager wifiManager;

  bool res;
  res = wifiManager.autoConnect(newHostname.c_str());

  if(!res) 
  {
      Serial.println("Failed to connect.");
      ESP.restart();
  } 
  else
  {
    Serial.println("Connected to Wifi.");
  }

  // Start the webserver
  server.begin();

  // connect to the MQTT broker
  pubsubClient.setServer(mqtt_server, 1883);
  pubsubClient.setCallback(callback);

  emon1.current(calibration); 
  Serial.println("**********************");

  //need to discard first few messages from the sensor as they are wildly inaccurate
  for (int i = 0; i<20; i++)
  {
    emon1.calcIrms(1480);
    delay(10);
  }



  // reset heartbeat timer
  LastMsg = millis();

} // end of setup

void loop() {

/*** handle web page requests START ***/
  WiFiClient wifiClient = server.available();   // Listen for incoming clients

  if (wifiClient) 
  {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (wifiClient.connected() && currentTime - previousTime <= timeoutTime) 
    { // loop while the client's connected
      currentTime = millis();         
      if (wifiClient.available()) {             // if there's bytes to read from the client,
        char c = wifiClient.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') 
        {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) 
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            wifiClient.println("HTTP/1.1 200 OK");
            wifiClient.println("Content-type:text/html");
            wifiClient.println("Connection: close");
            wifiClient.println();

          
            
            // Display the HTML web page
            wifiClient.println("<!DOCTYPE html><html>");
            wifiClient.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            wifiClient.println("<link rel=\"icon\" href=\"data:,\">");

            wifiClient.println("<style>");
            wifiClient.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            wifiClient.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            wifiClient.println("</style></head>");

            wifiClient.println("<body><h1>Power Monitor Configuration</h1>");        
            wifiClient.println("<p>Hello World</p>");            
            wifiClient.println("</body></html>");
            
            // The HTTP response ends with another blank line
            wifiClient.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    wifiClient.stop();
  }  

  /*** handle web page requests END ***/

  
  if (!pubsubClient.connected()) 
  {
    reconnect();
  } // end if
  pubsubClient.loop();


  // read A/D values and store in value, averaged over 10 readings
  Value = 0;
  for (int j = 0; j<10; j++)
  {
    Value = Value + emon1.calcIrms(1480);
  } 
  Value = Value / 10;

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
    pubsubClient.publish(InStatus, Report_array);

    //Output to Serial Monitor what we have sent
    Serial.print(String(loopCount)); Serial.print(": Published To topic: "); Serial.print(InStatus); Serial.print(" - Report Sent: "); Serial.println(Report_array);

    // only used to make the LED flash visable
    delay(10);

    digitalWrite(Network_LED, LOW);

  } // end of heartbeat timer
  
  loopCount++; //increment counter

} // end of loop
