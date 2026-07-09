#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NSUT_WIFI";
const char* password = "";

WebServer server(80);

// Instant-Trigger Earthquake Alarm (Zero Delay)
const int TILT_PIN = 14;   // Pin connected to Tilt Sensor DO
const int BUZZER_PIN = 13; // Pin connected to Buzzer Signal/IO

// --- CALIBRATION CONFIGURATION ---
// If the buzzer sounds when everything is still, change this to HIGH
const int SHAKE_DETECTED_STATE = LOW; 

String alarmStatus = "SAFE";

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'>";
  html += "<title>Earthquake Detector</title>";
  html += "<style>body{font-family: Arial; text-align: center; margin-top: 50px;";
  if (alarmStatus == "ALARM") {
    html += "background-color: red; color: white;";
  } else {
    html += "background-color: #d4edda; color: #155724;";
  }
  html += "}</style></head><body>";
  html += "<h1>Earthquake Monitor</h1>";
  html += "<h2>Status: " + alarmStatus + "</h2>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(TILT_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();

  Serial.println("System Ready. Instant real-time monitoring active.");
}

void loop() {
  server.handleClient();

  // Read the sensor instantly with zero blocking delays
  if (digitalRead(TILT_PIN) == SHAKE_DETECTED_STATE) {
    alarmStatus = "ALARM";
    
    // METHOD 1: For Active Buzzers (Continuous, solid tone)
    digitalWrite(BUZZER_PIN, HIGH); 
    
    // METHOD 2: For Passive Buzzers (Deep "Faahh" lower pitch horn)
    // If your buzzer sounds like a click instead of a tone, uncomment the line below
    // and comment out the digitalWrite line above.
    // tone(BUZZER_PIN, 350); 

  } else {
    alarmStatus = "SAFE";
    // Instantly turn off the sound the microsecond shaking stops
    digitalWrite(BUZZER_PIN, LOW);
    // noTone(BUZZER_PIN); // Uncomment this if you are using Method 2
  }
}