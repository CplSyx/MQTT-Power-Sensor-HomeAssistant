// https://arduino.stackexchange.com/questions/77770/confuse-about-progmem-and-r

const char index_html[] PROGMEM = R"rawhtml(

<!DOCTYPE html><html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">

<style>
html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
</style></head>

<body><h1>Power Monitor Configuration</h1>        
<p>Configuration</p>            
<div class="">
    <div class="">
        <label for="wifiSSID" class="">Wifi SSID<span class="">*</span></label>
        <input type="text" class="" name="wifiSSID" id="wifiSSID" required="required" value="%WIFISSID%">
    </div>
    <div class="">
        <label for="wifiPassword" class="">Wifi Password<span class="">*</span></label>
        <input type="text" class="form-control" name="wifiPassword" id="wifiPassword" required="required" value="%WIFIPASSWORD%">
    </div>

    <div class="">
        <label for="mqttURL" class="">MQTT URL<span class="">*</span></label>
        <input type="text" class="" name="mqttURL" id="mqttURL" required="required" value="%MQTTURL%">
    </div>

    <div class="">
        <label for="mqttPort" class="">MQTT Port<span class="">*</span></label>
        <input type="text" class="" name="mqttPort" id="mqttPort" required="required" value="%MQTTPORT%">
    </div>

    <div class="">
        <label for="mqttUsername" class="">MQTT Username<span class="">*</span></label>
        <input type="text" class="" name="mqttUsername" id="mqttUsername" required="required" value="%MQTTUSERNAME%">
    </div>

    <div class="">
        <label for="mqttPassword" class="">MQTT Password<span class="">*</span></label>
        <input type="text" class="" name="mqttPassword" id="mqttPassword" required="required" value="%MQTTPASSWORD%">
    </div>

    <div class="">
        <label for="newCalibration" class="">Calibration Value<span class="">*</span></label>
        <input type="text" class="" name="newCalibration" id="newCalibration" required="required" value="%CALIBRATION%">
    </div>

    <div class="">
        <button type="submit" class="" name="submitButton" value="save" style="" id="submitButton">Save Changes</button>
    </div>
</div>
</body></html>

)rawhtml";