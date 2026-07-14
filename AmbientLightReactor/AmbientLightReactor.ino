#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi Credentials
const char* ssid = "NSUT_WIFI";
const char* password = "";

WebServer server(80);

// Pin Definitions
const int LDR_DIGITAL_PIN = 34; // Connected to 'OUT' pin on your module
const int RED_PIN = 25;         // RGB Module Red
const int GREEN_PIN = 26;       // RGB Module Green
const int BLUE_PIN = 27;        // RGB Module Blue

String currentStatus = "Unknown";

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='2'>";
  html += "<title>Ambient Light Reactor</title>";
  html += "<style>body{font-family: Arial; text-align: center; margin-top: 50px;}</style></head><body>";
  html += "<h1>Ambient Light Reactor Status</h1>";
  html += "<h2>Current State: " + currentStatus + "</h2>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LDR_DIGITAL_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Connect to Wi-Fi
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
}

void loop() {
  server.handleClient();

  static int lastState = -1;
  // Read the digital state from the 'OUT' pin
  int isDark = digitalRead(LDR_DIGITAL_PIN);

  if (isDark != lastState) {
    if (isDark == LOW) {
      // ROOM IS DARK: Turn on a cozy ambient color (e.g., Warm Cyan/Blue)
      analogWrite(RED_PIN, 100);
      analogWrite(GREEN_PIN, 150);
      analogWrite(BLUE_PIN, 255);
      currentStatus = "Dark -> Ambient Reactor ON";
      Serial.println("Status: Dark -> Ambient Reactor ON");
    } else {
      // ROOM IS BRIGHT: Turn off the RGB reactor
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);
      currentStatus = "Bright -> Ambient Reactor OFF";
      Serial.println("Status: Bright -> Ambient Reactor OFF");
    }
    lastState = isDark;
  }
}