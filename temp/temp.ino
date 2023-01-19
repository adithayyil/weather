#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define VIN 5
#define prResistance 10000

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // I2C Address 0x3c
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int tempSensorPin = A0;
const int prSensorPin = A1;
const int motorPin = A2;
const float motorKvValue = 3;   // TODO: Look for datasheet for the specific motor i'm using to find the exact Kv

float temperature;
float lightIntensity;
float windSpeed;


void setup() {
  Serial.begin(9600);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);

}

float calculateWindSpeed() {
  float motorValue = analogRead(motorPin);

  float VOUT_MOTOR = motorValue * (VIN / float(1023));  // Convert ADC value to volts
  float windSpeed = motorKvValue * VOUT_MOTOR;         // Convert from volts to speed (speed = kv of motor * voltage)

  return windSpeed;
}

float calculateLightIntensity() {
  float prSensorValue = analogRead(prSensorPin);

  
  float VOUT_LUM = float(prSensorValue) * (VIN / float(1023));  // Convert ADC value to volts
  float RLDR = (prResistance * (VIN - VOUT_LUM)) / VOUT_LUM;   // Convert from volts to resistance
  float lux = 500 / (RLDR / 1000);                            // Convert resitance to lumen

  return lux;
}


float calculateTemp() {
  float tempSensorValue = analogRead(tempSensorPin);
 
  float VOUT_TEMP = float(tempSensorValue) * VIN / float(1023) ;   // Convert ADC value to volts
  float celsius = VOUT_TEMP / .02 ;                               // Convert from Volts to Celsius (20mV per degree)
  float fahrenheit = (celsius  * 9.0 / 5.0) + 32;                // Convert from Celsius to Fahrenheit

  return fahrenheit;
}

void loop() {
   temperature = calculateTemp();
   lightIntensity = calculateLightIntensity();
   windSpeed = calculateWindSpeed();

  Serial.println("Temp: " + String(temperature) + " F");
  Serial.println("Light Intensity: " + String(lightIntensity) + " Lumens");
  Serial.println("Wind Speed: " + String(windSpeed) + " m/s");

  display.clearDisplay();  // Updates Display

  // Displays Title
  display.setTextSize(1.6);
  display.setCursor(0, 0);
  display.println("All-in-One Sensor");
  
  // Display Temperature
  display.setTextSize(1);
  display.setCursor(0,20);
  display.print("TMP: ");
  display.setTextSize(1);
  display.print(temperature);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(1);
  display.print("F");
  
  // Display Light Intensity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("LUM: ");
  display.setTextSize(1);
  display.print(lightIntensity);
  display.print(" Lumens"); 

  // Display Light Intensity
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("WIND: ");
  display.setTextSize(1);
  display.print(windSpeed);
  display.print(" m/s"); 
  
  display.display(); 

  delay(1000);
}
