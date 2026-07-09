#include <DHT.h>
#include <ESP32Servo.h>

#define DHTPIN 4          // GPIO 4 for DHT11 data
#define DHTTYPE DHT11     
#define SERVOPIN 18       // GPIO 18 for Servo signal

DHT dht(DHTPIN, DHTTYPE);
Servo fanServo;

// --- CONFIGURATION ---
const float TEMP_MIN = 25.0;   // Temp where fan starts moving (Slowest sweep)
const float TEMP_MAX = 35.0;   // Temp where fan hits maximum sweeping speed

// Servo timing variables (in milliseconds)
const int MAX_DELAY = 45;      // Slower step delay (Low RPM)
const int MIN_DELAY = 8;       // Faster step delay (High RPM)
const int STEP_SIZE = 5;       // Moving 5 degrees per step for smooth motion

void setup() {
  Serial.begin(115200); 
  dht.begin();
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  fanServo.setPeriodHertz(50); 
  fanServo.attach(SERVOPIN, 500, 2400); 
  
  fanServo.write(0); // Start at home position
  Serial.println("System Initialized: Variable Speed Servo Fan active.");
}

void loop() {
  // Read temperature from DHT11
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Error: Failed to read from DHT11 sensor!");
    delay(2000);
    return; 
  }

  float estimatedRPM = 0.0;

  // Check if temperature is above the starting threshold
  if (temp >= TEMP_MIN) {
    // Calculate how hot the room is as a percentage (0.0 to 1.0) between our min and max limits
    float tempPercentage = (temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
    if (tempPercentage > 1.0) tempPercentage = 1.0; // Cap it at 100%

    // Calculate the delay between steps. Higher temp percentage = smaller delay = faster speed.
    int sweepDelay = MAX_DELAY - (tempPercentage * (MAX_DELAY - MIN_DELAY));

    // --- MATH FOR RPM TELEMETRY ---
    // A full round trip (0 to 180, and 180 to 0) equals 360 degrees of physical movement.
    // Total steps per full cycle = (180 / STEP_SIZE) * 2 = 72 steps.
    // Total time for one full cycle in milliseconds = 72 steps * sweepDelay.
    // RPM = 60,000 milliseconds (1 minute) divided by the total cycle time.
    float totalCycleTimeMs = (360 / STEP_SIZE) * 2 * sweepDelay;
    estimatedRPM = 60000.0 / totalCycleTimeMs;

    // --- PRINT DATA BEFORE SWEEP ---
    Serial.print("Temp: ");
    Serial.print(temp, 1);
    Serial.print(" °C | Step Delay: ");
    Serial.print(sweepDelay);
    Serial.print("ms | Fan Speed: ");
    Serial.print(estimatedRPM, 1);
    Serial.println(" RPM");

    // --- EXECUTE ONE OSCILLATION CYCLE ---
    // Sweep forward from 0 to 180 degrees
    for (int angle = 0; angle <= 180; angle += STEP_SIZE) {
      fanServo.write(angle);
      delay(sweepDelay);
    }
    // Sweep backward from 180 to 0 degrees
    for (int angle = 180; angle >= 0; angle -= STEP_SIZE) {
      fanServo.write(angle);
      delay(sweepDelay);
    }
    
  } else {
    // If room is cool, turn off/idle the fan at 0 degrees
    fanServo.write(0);
    estimatedRPM = 0.0;

    Serial.print("Temp: ");
    Serial.print(temp, 1);
    Serial.print(" °C | Fan Status: IDLE | Speed: ");
    Serial.print(estimatedRPM, 1);
    Serial.println(" RPM");
    
    delay(2000); // Wait 2 seconds before checking temperature again while idling
  }
}