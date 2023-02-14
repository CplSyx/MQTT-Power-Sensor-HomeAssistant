// https://arduino.stackexchange.com/questions/77770/confuse-about-progmem-and-r

const char index_html[] PROGMEM = R"rawhtml(

<!DOCTYPE html>
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">

<style>
html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}

</style>
<link rel="stylesheet" href="https://netdna.bootstrapcdn.com/bootstrap/3.3.2/css/bootstrap.min.css">
</head>



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
        <label for="mqttURL" class="formbuilder-text-label">MQTT Host IP<span class="formbuilder-required">*</span></label>
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
        <label for="mqttTopic" class="formbuilder-text-label">MQTT Topic<span class="formbuilder-required">*</span></label>
        <input type="text" class="form-control" name="mqttTopic" id="mqttTopic" required="required" value="%MQTTTOPIC%">
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

  <form class="form-horizontal" action="/restart" method="post">
  <fieldset>
  <button type="submit" class="form-control" name="restartButton" value="restart" style="" id="restartButton">Restart</button>
  </fieldset>
  </form>

</body>
</html>

)rawhtml";

const char restart_html[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">

<script>
  function sendResetRequest() 
  {
    var http = new XMLHttpRequest();
    http.open("POST", "/restart", true);
    http.setRequestHeader("Content-type","application/x-www-form-urlencoded");
    var params = "restart=true";
    http.send(params);
    redirect();
  }

  function redirect () {
    setTimeout(myURL, 5000);
    var result = document.getElementById("result");
    result.innerHTML = "<b> Restarting... </b>";
  }

  function myURL() {
      document.location.href = window.location.origin;
  }
</script>

<style>
html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}

</style>
<link rel="stylesheet" href="https://netdna.bootstrapcdn.com/bootstrap/3.3.2/css/bootstrap.min.css">
</head>



<body>
<h1>Power Monitor Configuration</h1>  
<div id = "result">ARE YOU SURE?<br>
  <form class="form-horizontal" action="" method="post">
  <fieldset>

  <div class="">
  <button type="button" class="form-control" name="restartButton" style="" id="restartButton" onclick="sendResetRequest();">Yes - Restart Device</button>
  </div>

  <div class="">
  <button type="button" class="form-control" name="backButton" style="" id="backButton" onclick="window.history.back();">No - Go Back</button>
  </div>

  </fieldset>
  </form>
</div>
  </body>
</html>
  )rawhtml";