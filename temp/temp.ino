int tempSensorPin = A0;
int prSensorPin = A1;

float temperature;
float lightIntensity;


#define prResistance 10000

void setup() {
  Serial.begin(9600);
}

float lightIntestityCalculations() {
  float prSensorValue = analogRead(prSensorPin);

  
  float Vout = float(prSensorValue) * (5 / float(1023));     // Conversion analog to voltage
  float RLDR = (prResistance * (5 - Vout))/Vout;   // Conversion voltage to resistance
  float lux = 500/(RLDR/1000);                     // Conversion resitance to lumen
  return lux;
}


float tempCalculations() {
  float tempSensorValue = analogRead(tempSensorPin);
 
  float volts = tempSensorValue * 5.0 / 1024 ;       // Convert ADC value to volts
  float celsius = volts / .02 ;                     // Convert from Volts to Celsius (20mV per degree)
  float fahrenheit = (celsius  * 9.0 / 5.0) + 32;  // Convert from Celsius to Fahrenheit

  return fahrenheit;
}

void loop() {
   temperature = tempCalculations();
   lightIntensity = lightIntestityCalculations();

  //Serial.println(temperature)
  Serial.println(lightIntensity);

  delay(1000);
}
