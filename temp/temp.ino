#define prResistance 10000
#define VIN 5

const int tempSensorPin = A0;
const int prSensorPin = A1;
const int motorPin = A2;
const float motorKvValue = 3;   // TODO: Look for datasheet for the specific motor i'm using to find the exact Kv

float temperature;
float lightIntensity;
float windSpeed;

void setup() {
  Serial.begin(9600);
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

  delay(1000);
}
