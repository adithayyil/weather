#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <FS.h>

#define tempGPIOPin D3
#define tempAnalogPin A0

#define VIN 3.3

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // I2C Address 0x3c

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float temperatureInCelsius;
float temperatureInFahrenheit;

// Replace with your network credentials
const char* ssid = "xxxx";
const char* password = "xxxx";

AsyncWebServer server(80);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  pinMode(tempGPIOPin, OUTPUT);

  // // Initialize SPIFFS
  // if(!SPIFFS.begin()){
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   return;
  // }

  // // Connect to Wi-Fi
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi..");
  // }

  // // Print ESP32 Local IP Address
  // Serial.println(WiFi.localIP());

  // // Route for root / web page
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SPIFFS, "/index.html");
  // });
  // server.on("/temperatureInCelsius", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", String(calculateTemp(true)).c_str());
  // });
  // server.on("/temperatureInFahrenheit", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", String(calculateTemp(false)).c_str());
  // });

  // // Start server
  // server.begin();
}

float calculateTemp(bool isCelsius) {
  digitalWrite(tempGPIOPin, HIGH);
  float tempSensorValue = analogRead(tempAnalogPin);

  float VOUT_TEMP = float(tempSensorValue) * VIN / float(1023);  // Convert ADC value to volts
  float celsius = VOUT_TEMP / .02;                               // Convert from Volts to Celsius (20mV per degree)
  float fahrenheit = (celsius * 9.0 / 5.0) + 32;                 // Convert from Celsius to Fahrenheit

  if (isCelsius) {
    return celsius;
  }

  else {
    return fahrenheit;
  }
}

void loop() {
  temperatureInCelsius = calculateTemp(true);
  temperatureInFahrenheit = calculateTemp(false);

  Serial.println("Temp: " + String(temperatureInFahrenheit) + " F");
  Serial.println("Temp: " + String(temperatureInCelsius) + " C");

  display.clearDisplay();  // Updates Display

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


  display.display();


  delay(1000);
}
