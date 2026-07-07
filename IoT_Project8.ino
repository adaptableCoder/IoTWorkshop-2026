// Instant-Trigger Earthquake Alarm (Zero Delay)
const int TILT_PIN = 14;   // Pin connected to Tilt Sensor DO
const int BUZZER_PIN = 13; // Pin connected to Buzzer Signal/IO

// --- CALIBRATION CONFIGURATION ---
// If the buzzer sounds when everything is still, change this to HIGH
const int SHAKE_DETECTED_STATE = LOW; 

void setup() {
  Serial.begin(115200);
  pinMode(TILT_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("System Ready. Instant real-time monitoring active.");
}

void loop() {
  // Read the sensor instantly with zero blocking delays
  if (digitalRead(TILT_PIN) == SHAKE_DETECTED_STATE) {
    
    // METHOD 1: For Active Buzzers (Continuous, solid tone)
    digitalWrite(BUZZER_PIN, HIGH); 
    
    // METHOD 2: For Passive Buzzers (Deep "Faahh" lower pitch horn)
    // If your buzzer sounds like a click instead of a tone, uncomment the line below
    // and comment out the digitalWrite line above.
    // tone(BUZZER_PIN, 350); 

  } else {
    // Instantly turn off the sound the microsecond shaking stops
    digitalWrite(BUZZER_PIN, LOW);
    // noTone(BUZZER_PIN); // Uncomment this if you are using Method 2
  }
}