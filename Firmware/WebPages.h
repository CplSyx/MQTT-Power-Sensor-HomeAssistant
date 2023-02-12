// https://arduino.stackexchange.com/questions/77770/confuse-about-progmem-and-r

const char index_html[] PROGMEM = R"rawhtml(

<!DOCTYPE html>
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">

<style>
html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}

</style></head>

<link rel="stylesheet" href="https://netdna.bootstrapcdn.com/bootstrap/3.3.2/css/bootstrap.min.css">

<body>
<h1>Power Monitor Configuration</h1>        
<p>%OUTPUTMESSAGE%</p>
<form class="form-horizontal" action="/update" method="post">
<fieldset>

<!-- Form Name -->
<legend>Configuration</legend>       
    <div class="">
        <label for="wifiSSID" class="formbuilder-text-label">Wifi SSID<span class="formbuilder-required">*</span></label>
        <input type="text" class="form-control" name="wifiSSID" id="wifiSSID" required="required" value="%WIFISSID%">
    </div>
    <div class="">
        <label for="wifiPassword" class="formbuilder-text-label">Wifi Password<span class="formbuilder-required">*</span></label>
        <input type="password" class="form-control" name="wifiPassword" id="wifiPassword" required="required" value="%WIFIPASSWORD%">
    </div>

    <div class="">
        <label for="mqttURL" class="formbuilder-text-label">MQTT URL<span class="formbuilder-required">*</span></label>
        <input type="text" class="form-control" name="mqttURL" id="mqttURL" required="required" value="%MQTTURL%">
    </div>

    <div class="">
        <label for="mqttPort" class="formbuilder-text-label">MQTT Port<span class="formbuilder-required">*</span></label>
        <input type="text" class="form-control" name="mqttPort" id="mqttPort" required="required" value="%MQTTPORT%">
    </div>

    <div class="">
        <label for="mqttUsername" class="formbuilder-text-label">MQTT Username<span class="formbuilder-required">*</span></label>
        <input type="text" class="form-control" name="mqttUsername" id="mqttUsername" required="required" value="%MQTTUSERNAME%">
    </div>

    <div class="">
        <label for="mqttPassword" class="formbuilder-text-label">MQTT Password<span class="formbuilder-required">*</span></label>
        <input type="password" class="form-control" name="mqttPassword" id="mqttPassword" required="required" value="%MQTTPASSWORD%">
    </div>

    <div class="">
        <label for="newCalibration" class="formbuilder-text-label">Calibration Value<span class="formbuilder-required">*</span></label>
        <input type="text" class="form-control" name="newCalibration" id="newCalibration" required="required" value="%CALIBRATION%">
    </div>

    <div class="">
        <button type="submit" class="form-control" name="submitButton" value="save" style="" id="submitButton">Save Changes</button>
    </div>

</fieldset>
</form>
</body>
</html>

)rawhtml";