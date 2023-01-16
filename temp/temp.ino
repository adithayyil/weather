int tempSensorPin = A0;

void setup() {
  Serial.begin(9600);
}

void tempCalculations() {
  int tempSensorValue = analogRead(tempSensorPin);
  // Convert ADC value to volts
  float volts = tempSensorValue * 5.0 / 1024 ;
  // Convert from Volts to Celsius (20mV per degree)
  float celsius = volts / .02 ;
  // Convert from Celsius to Fahrenheit
  float fahrenheit = (Celsius  * 9.0 / 5.0) + 32;

  return fahrenheit;
}

void loop() {
  temp

  Serial.println(tempCalculations())

  delay(200);
}