// https://arduino.stackexchange.com/questions/77770/confuse-about-progmem-and-r

const char index_html[] PROGMEM = R"rawhtml(

<!DOCTYPE html>
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>Power Monitor Configuration</title>
<style>
:root{
	--primarycolor:#1fa3ec;
}
body {
    text-align: center;
    font-family: Helvetica;
}
div,
input,select {
    padding: 5px;
    font-size: 1em;
    margin: 5px 0;

}
div{
	margin: 5px 0;
}
input,button,select,.msg{
	border-radius:.3rem;

}
button,input[type="button"],input[type="submit"] {
    border: 0;
    background-color: var(--primarycolor);
    color: #fff;
    line-height: 2.4rem;
    font-size: 1.2rem;
}
</style>
</head>

<body>
  <h1>Power Monitor Configuration</h1>        
  <p>%OUTPUTMESSAGE%</p>
  <form class="form-horizontal" action="/update" method="post">
  <fieldset>
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

    </fieldset>

    <div class="">
        <button type="submit" class="form-control" name="submitButton" value="save" style="" id="submitButton">Save Changes</button>        
    </div>

  </form>

  <form class="form-horizontal" action="/restart" method="post">
  <button type="submit" class="form-control" name="restartButton" value="restart" style="" id="restartButton">Restart</button>
  </form>

</body>
</html>

)rawhtml";

const char update_html[] PROGMEM = R"rawhtml(

<!DOCTYPE html>
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>Power Monitor Configuration</title>
<style>
:root{
	--primarycolor:#1fa3ec;
}
body {
    text-align: center;
    font-family: Helvetica;
}
div,
input,select {
    padding: 5px;
    font-size: 1em;
    margin: 5px 0;

}
div{
	margin: 5px 0;
}
input,button,select,.msg{
	border-radius:.3rem;

}
button,input[type="button"],input[type="submit"] {
    border: 0;
    background-color: var(--primarycolor);
    color: #fff;
    line-height: 2.4rem;
    font-size: 1.2rem;
}
</style>
</head>



<body>
  <h1>Power Monitor Configuration</h1>        
  <p>%OUTPUTMESSAGE%</p>

  <form class="form-horizontal" action="/restart" method="post">
  <button type="submit" class="form-control" name="restartButton" value="restart" style="" id="restartButton">Restart</button>
  </form>

</body>
</html>

)rawhtml";

const char restart_html[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>Power Monitor Configuration</title>
<style>
:root{
	--primarycolor:#1fa3ec;
}
body {
    text-align: center;
    font-family: Helvetica;
}
div,
input,select {
    padding: 5px;
    font-size: 1em;
    margin: 5px 0;

}
div{
	margin: 5px 0;
}
input,button,select,.msg{
	border-radius:.3rem;

}
button,input[type="button"],input[type="submit"] {
    border: 0;
    background-color: var(--primarycolor);
    color: #fff;
    line-height: 2.4rem;
    font-size: 1.2rem;
}
</style>

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
</head>



<body>
<h1>Power Monitor Configuration</h1>  
<div id = "result">ARE YOU SURE?<br>
  <form class="form-horizontal" action="" method="post">

  <div class="">
  <button type="button" class="form-control" name="restartButton" style="" id="restartButton" onclick="sendResetRequest();">Yes - Restart Device</button>
  </div>

  <div class="">
  <button type="button" class="form-control" name="backButton" style="" id="backButton" onclick="myURL();">No - Go Back</button>
  </div>

  </form>
</div>
  </body>
</html>
  )rawhtml";