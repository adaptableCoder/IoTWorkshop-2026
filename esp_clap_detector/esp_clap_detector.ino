const int micPin = 34;      // AO connected to GPIO34
const int threshold = 0; // Change this after testing

void setup() {
  Serial.begin(115200);
}

void loop() {
  int sound = analogRead(micPin);

  Serial.println(sound);    // View the analog value

  if (sound > threshold) {
    Serial.println("Clap detected!");
    delay(300);             // Prevent repeated detections
  }

  delay(10);
}