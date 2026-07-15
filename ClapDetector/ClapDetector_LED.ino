#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NSUT_WIFI";
const char* password = "";

WebServer server(80);

const int micPin = 34;      // AO connected to GPIO34
const int threshold = 2000; 
int clapCount = 0;
int ledp=2;
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='2'>";
  html += "<title>Clap Detector</title>";
  html += "<style>body{font-family: Arial; text-align: center; margin-top: 50px;}</style></head><body>";
  html += "<h1>Clap Detector Status</h1>";
  html += "<h2>Total Claps Detected: " + String(clapCount) + "</h2>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledp, OUTPUT);

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

  int sound = analogRead(micPin);
  
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.println(sound);    // View the analog value periodically
    lastPrint = millis();
    digitalWrite(ledp, 0);
  }
  
  if (sound > threshold) {
    Serial.println("Clap detected!");
    digitalWrite(ledp, 1000);
    clapCount++;
    // non-blocking delay
    unsigned long start = millis();
    while (millis() - start < 300) { server.handleClient(); delay(1); }
  }
}
