/*
    Title:        MQTT 1 Way Mains Power Sensor Demo
    Date:         4th September 2021
    Version:      1
    Description:  Sample code
    Device:       ESP8266
    WiFi:		  See "WiFi_Settings.h" for wifi AP and password
    MQTT:		  See "MQTT_Settings.h" for MQTT broker and topic
*/

/* ********************************** Compiler settings, un-comment to use ************************** */
//#define Fixed_IP                      // un-comment to use a fixed IP address to speed up development
//#ifdef Watchdog_ON                    // watchdog items, comment out if not used
#define Print_Report_Level_1          // un-comment for option
#define Print_Report_Level_2          // un-comment for option, report received MQTT message
#define Print_Report_Level_3          // un-comment for option
/* ************************************************************************************************** */



#include <ESP8266WiFi.h>              // needed for EPS8266
#include <WiFiClient.h>               // WiFi client

// custom settings files
#include "Secrets.h"                  // Usernames and passwords
#include "Wifi_Settings.h"            // custom Wifi settings
#include "MQTT_Settings.h"            // MQTT broker and topic
#include "Project_Settings.h"         // board specific details.

// incude WiFi and MQTT functions
WiFiClient espClient;                 // for ESP8266 boards
#include "PubSubClient.h"             // http://pubsubclient.knolleary.net/api.html
PubSubClient client(espClient);       // ESP pubsub client
#include "WiFi_Functions.h"           // read wifi data
#include "MQTT_Functions.h"           // MQTT Functions

// library for the MLP191020 PCB
//#include "MLP191020.h"
// make an instance of MLP191020
//MLP191020 My_PCB(Cal_value);

// EmonLibrary
#include "EmonLib_CurrentOnly.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance



void setup() {
  Serial.begin(115200);

#ifdef Watchdog_ON
  // watchdog items, comment out if not used
  secondTick.attach(1, ISRwatchdog);
#endif

  // I/O
  pinMode(Network_LED, OUTPUT);
  digitalWrite(Network_LED, LOW);

  // connect to WiFi access point
  Get_Wifi();

  // connect to the MQTT broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //need to discard first few messages from the sensor as they are wildly inaccurate
  for (int i = 0; i<20; i++)
  {
    //My_PCB.power_sample(); 
    delay(10);
  }

  emon1.current(2, 87.5); 
  Serial.println("**********************");

  // reset heartbeat timer
  LastMsg = millis();

} // end of setup

void loop() {

  if (!client.connected()) {
    reconnect();
  } // end if
  client.loop();

  // read A/D values and store in value
  Value = emon1.calcIrms(1480);//My_PCB.power_sample();

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

#ifdef Print_Report_Level_2
    // display a report when publishing
    Serial.print("Published To topic: "); Serial.print(InStatus); Serial.print("  -  Report Sent: "); Serial.println(Report_array);
#endif

    digitalWrite(Network_LED, HIGH);
    // send a status report
    client.publish(InStatus, Report_array);

    /*double Irms = 0;
    for (int j = 0; j<10; j++)
    {
      Irms = Irms + emon1.calcIrms(1480);
    }  // Calculate Irms only
    Irms = Irms / 10;
    Serial.print("Current (Emon): ");
    Serial.println(Irms);		*/

    // only used to make the LED flash visable
    delay(10);

    digitalWrite(Network_LED, LOW);

  } // end of heartbeat timer

} // end of loop
