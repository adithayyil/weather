#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <FS.h>

#define tempAnalogPin A0

#define VIN 3.3

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // I2C Address 0x3c

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Initializes the display

String temperatureInCelsius;
String temperatureInFahrenheit;

// Frontend content for the webserver
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://code.highcharts.com/highcharts.js"></script>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
        html {
            font-family: Arial;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }

        body {
            min-width: 310px;
            max-width: 800px;
            height: 400px;
            margin: 0 auto;
        }

        p {
            font-size: 3.0rem;
        }

        h2 {
            font-family: Arial;
            font-size: 2.5rem;
            text-align: center;
        }
    </style>
</head>

<body>
    <h2>Adi's Room Data</h2>
    <p>
        <i class="fas fa-thermometer-half" style="color:#7D26CD;"></i>
        <span id="temperaturec">%TEMPERATUREC%</span>
        <sup class="units">&deg;C</sup>
    </p>
    <p>
        <i class="fas fa-thermometer-half" style="color:#7D26CD;"></i>
        <span id="temperaturef">%TEMPERATUREF%</span>
        <sup class="units">&deg;F</sup>
    </p>
    <div id="chart-temperature" class="container"></div>
</body>
<script>
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("temperaturec").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/temperaturec", true);
        xhttp.send();
    }, 1000);
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("temperaturef").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/temperaturef", true);
        xhttp.send();
    }, 1000);




    var chartT = new Highcharts.Chart({
        chart: { renderTo: 'chart-temperature' },
        title: { text: 'Graph (Degree Fahrenheit)' },
        series: [{
            showInLegend: false,
            data: []
        }],
        plotOptions: {
            line: {
                animation: false,
                dataLabels: { enabled: true }
            },
            series: { color: '#7D26CD' }
        },
        xAxis: {
            type: 'datetime',
            dateTimeLabelFormats: { second: '%H:%M:%S' }
        },
        yAxis: {
            title: { text: 'Temperature (Degree Fahrenheit)' }
        },
        credits: { enabled: false }
    });
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                var x = (new Date()).getTime(),
                    y = parseFloat(this.responseText);
                //console.log(this.responseText);
                if (chartT.series[0].data.length > 40) {
                    chartT.series[0].addPoint([x, y], true, true, true);
                } else {
                    chartT.series[0].addPoint([x, y], true, false, true);
                }
            }
        };
        xhttp.open("GET", "/temperature", true);
        xhttp.send();
    }, 1000);
</script>

</html>

</html>)rawliteral";

// Network credentials for local network
const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);  // Initalizes web server

// Replaces placeholder with TMP37P values
String processor(const String& var){
  if(var == "TEMPERATUREC"){
    return  temperatureInCelsius;
  }
  else if(var == "TEMPERATUREF"){
    return temperatureInFahrenheit;
  }
  return String();
}

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay(); // Set up display
  display.setTextColor(WHITE);  // "Clears" display

  // Initialize SPIFFS (loca)
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  // Route for Tempreture (Celsius)
  server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureInCelsius.c_str());
  });
  // Route for Tempreture (Fahrenheit)
  server.on("/temperaturef", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureInFahrenheit.c_str());
  });
  // Route for Tempreture Graph
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureInFahrenheit.c_str());
  });

  // Start server
  server.begin();
}

String calculateTemp(bool isCelsius) {
  float tempSensorValue = analogRead(tempAnalogPin);                // Reads analog value of TMP37F

  float VOUT_TEMP = float(tempSensorValue) * VIN / float(1023);   // Convert ADC value to volts
  float celsius = VOUT_TEMP / .02;                               // Convert from Volts to Celsius (20mV per degree)
  float fahrenheit = (celsius * 9.0 / 5.0) + 32;                // Convert from Celsius to Fahrenheit

  if (isCelsius) {
    return String(celsius);
  }

  else {
    return String(fahrenheit);
  }
}

void loop() {
  temperatureInCelsius = calculateTemp(true);        // Gets temp in Celsius
  temperatureInFahrenheit = calculateTemp(false);   // Gets temp in Fahrenheit

  // Serial.println("Temp: " + String(temperatureInFahrenheit) + " F");
  // Serial.println("Temp: " + String(temperatureInCelsius) + " C");

  display.clearDisplay();  // Clears Display

  // Displays Title
  display.setTextSize(1);
  display.setCursor(5, 0);
  display.println("Adi's Room Temp");

  // Display Temperature (°C)
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("TMP: ");
  display.setTextSize(1);
  display.print(temperatureInFahrenheit);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(1);
  display.print("F");

  // Display Temperature (°F)
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("TMP: ");
  display.setTextSize(1);
  display.print(temperatureInCelsius);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(1);
  display.print("C");


  display.display();  // Updates Display


  delay(1000);
}