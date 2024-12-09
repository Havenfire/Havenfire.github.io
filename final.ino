#include <Servo.h>

// Pin definitions
const int thermistorPin = A4; // Thermistor connected to analog pin A0
const int servoPin = 9;      // Servo connected to digital pin 9 (PWM pin)

// Constants for thermistor calculation
const float SERIES_RESISTOR = 10000.0; // 10k Ohm resistor
const float THERMISTOR_NOMINAL = 10000.0; // 10k Ohms at 25 degrees Celsius
const float TEMPERATURE_NOMINAL = 25.0; // Nominal temperature in Celsius
const float B_COEFFICIENT = 3965.0; // Beta coefficient of thermistor

// Target temperature range for movement
const float MIN_TEMP = 27.0; // Lower bound of temperature range
const float MAX_TEMP = 35.0; // Upper bound of temperature range

// Servo movement parameters
const int MIN_ANGLE = 0;      // Minimum servo angle
const int MAX_ANGLE = 180;    // Maximum servo angle
const int MAX_STEP_DELAY = 50; // Slowest movement delay
const int MIN_STEP_DELAY = 5;  // Fastest movement delay

// Create servo object
Servo myServo;

void setup() {
  myServo.attach(servoPin); // Attach the servo
  myServo.write(MIN_ANGLE); // Initialize servo to the starting position
  Serial.begin(9600); // Initialize serial monitor for debugging
}

void loop() {
  // Read thermistor value
  int analogValue = analogRead(thermistorPin);

  // Convert the analog value to resistance
  float resistance = SERIES_RESISTOR / (1023.0 / analogValue - 1.0);

  // Invert the calculation for temperature in Kelvin
  float temperatureK = 1.0 / (1.0 / (TEMPERATURE_NOMINAL + 273.15) -
                              (1.0 / B_COEFFICIENT) * log(resistance / THERMISTOR_NOMINAL));

  // Convert Kelvin to Celsius
  float temperatureC = temperatureK - 273.15;

  // Print values for debugging
  Serial.print("Analog Input: ");
  Serial.print(analogValue);
  Serial.print(" | Resistance (Ohms): ");
  Serial.print(resistance, 2); // Resistance
  Serial.print(" | Temperature (C): ");
  Serial.println(temperatureC);

  // Check if the temperature is within the range
  if (temperatureC >= MIN_TEMP && temperatureC <= MAX_TEMP) {
    // Map the temperature to a delay value
    int stepDelay = map(temperatureC, MIN_TEMP, MAX_TEMP, MAX_STEP_DELAY, MIN_STEP_DELAY);
    stepDelay = constrain(stepDelay, MIN_STEP_DELAY, MAX_STEP_DELAY); // Ensure it stays within bounds

    // Oscillate the servo between MIN_ANGLE and MAX_ANGLE with dynamic speed
    for (int angle = MIN_ANGLE; angle <= MAX_ANGLE; angle++) {
      myServo.write(angle);
      delay(stepDelay);
    }
    for (int angle = MAX_ANGLE; angle >= MIN_ANGLE; angle--) {
      myServo.write(angle);
      delay(stepDelay);
    }
  } else {
    // Keep the servo still at MIN_ANGLE
    myServo.write(MIN_ANGLE);
    delay(500); // Small delay to avoid rapid polling
  }
}
