const int sensorPin = A0;  // TMP36 sensor connected to analog pin A0
const int ledPin = 13;     // Built-in LED connected to digital pin 13
int sensorValue = 0;       // Variable to store the sensor input value

void setup() {
  Serial.begin(9600);      // Initialize serial communication at 9600 baud
  pinMode(ledPin, OUTPUT); // Set the LED pin as an output
}

void loop() {
  sensorValue = analogRead(sensorPin); // Read the value from the sensor
  float voltage = sensorValue * (5.0 / 1023.0); // Convert the sensor value to voltage
  float temperatureC = (voltage - 0.5) * 100.0; // Convert the voltage to Celsius temperature

  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" C");

  // Control the LED based on temperature value
  if (temperatureC > 25.0) {  // If temperature is higher than 25 degrees Celsius
    digitalWrite(ledPin, HIGH); // Turn on the LED
  } else {
    digitalWrite(ledPin, LOW);  // Turn off the LED
  }

  delay(1000); // Wait for 1 second
}
