#define prResistance 10000
#define VIN 5

const int tempSensorPin = A0;
const int prSensorPin = A1;

float temperature;
float lightIntensity;

void setup() {
  Serial.begin(9600);
}

float lightIntestityCalculations() {
  float prSensorValue = analogRead(prSensorPin);

  
  float VOUT = float(prSensorValue) * (VIN / float(1024));  // Convert ADC value to volts
  float RLDR = (prResistance * (VIN - VOUT))/VOUT;         // Convert from volts to resistance
  float lux = 500/(RLDR/1000);                            // Convert resitance to lumen

  return lux;
}


float tempCalculations() {
  float tempSensorValue = analogRead(tempSensorPin);
 
  float VOUT_TEMP = tempSensorValue * VIN / float(1024) ;   // Convert ADC value to volts
  float celsius = volts / .02 ;                            // Convert from Volts to Celsius (20mV per degree)
  float fahrenheit = (celsius  * 9.0 / 5.0) + 32;         // Convert from Celsius to Fahrenheit

  return fahrenheit;
}

void loop() {
   temperature = tempCalculations();
   lightIntensity = lightIntestityCalculations();

  //Serial.println(temperature)
  Serial.println(lightIntensity);

  delay(1000);
}
