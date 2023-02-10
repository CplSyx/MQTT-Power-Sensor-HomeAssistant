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
<p>Hello World</p>            
</body></html>

)rawhtml";