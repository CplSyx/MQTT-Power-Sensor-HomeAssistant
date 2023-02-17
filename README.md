# ESP8266 Power Sensor over MQTT 

### Features / Notes
- Web-based configuration, including "AP" mode if no WiFi connection found. No in-code config required.
- JSON formatted MQTT output, enabling compatibility with HomeAssistant and similar applications
- Forked and reworked from Mottramlabs/MQTT-Power-Sensor

### Requirements
- Wemos D1 Mini
- [MottramLabs Wemos D1 Mini Version ESP8266 Mains Power Sensor](https://www.mottramlabs.com/esp_products.html) or equivalent supporting circuitry
- YHDC SCT-013-000 Current Transformer or equivalent

## Overview
The sensor PCB uses a split core Current Transformer (CT) to sense the AC current flowing through a mains cable, a typical CT would be the SCT-013-000 (Current Type) capable of measuring up to 100A available on eBay. The current transformer output is applied to a burden resistor (22 ohm) which is biased at around half of the ESP8266's supply voltage (3.3V). The ESP8266's own A/D is then used to measure voltage developed across this burden resistor.  The A/D used in the ESP8266 has an input voltage range of 0-1V but the Wemos adds a resistor divider to allow for a 0-3.3V input range to the Wemos board. 

Included in this repository are the PCB files with schematic, a simple 3D printable case and firmware for the ESP8266.

## Setup / Installation
Configuration of the Arduino IDE for ESP8266 is not covered here.
Code compilation will require libraries:
- ESPAsyncTCP by me-no-dev (via GitHub [here](https://github.com/me-no-dev/ESPAsyncTCP))
- ESPAsyncWebServer by me-no-dev (via GitHub [here](https://github.com/me-no-dev/ESPAsyncWebServer))
- Preferences by Volodymyr Shymanskyy (via IDE library manager)
- WiFiManager by tzapu (via IDE library manager)

1. Flash Firmware.ino to your Wemos D1 Mini
2. Connect the hardware by attaching the breakout board
3. Plug in the CT.
4. Attach the CT to the wire to be measured. To avoid negative values, the power flow is from "back to front" if the printed text is on the front (check your CT as this is specific for the YHDC version). See Safety Note below.
5. Power on the board.
6. Connect to the WiFi network "PowerMonitor_xxxxxx" and configure WiFi.
7. Locate and go to the IP address of the board, and configure the remaining settings.
8. Restart!

### Safety Note :warning:

When connecting and disconnecting the CT to the circuit to be measured, ensure that it is also connected to the breakout board (and therefore the burden resistor).

Disconnection from a live circuit without the burden resistor will induce a high voltage on the secondary side - where the exposed connector is! Keep the CT connected to the burden resistor when attaching/detaching to avoid accidental damage or a shock.

### Calibration Value
The calibration value is obtained as
```
CT Ratio / Burden resistance
```
For the YHDC SCT-013-000, the CT Ratio printed on the front is 100A : 50mA, and the burden resistor on the breakout board is 22Ω
```
(100 / 0.05) / 22 = 90.9
```
Therefore we can expect to use 90.9 our value.

This can be tested against a known *resistive* source, such as an electrical heater, and adjusted as required.

## HomeAssistant setup
TBC
