#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NSUT_WIFI";
const char* password = "";

WebServer server(80);

const int touchSensorPin = 34;   // KY-036 analog out
const int buzzerPin = 25;        // Active buzzer pin
int threshold = 2000;            // Adjust as needed

String touchStatus = "WAITING";

// === Luis Fonsi - Despacito (Chorus Hook) ===
int melody[] = {
  // Des-pa-ci-to
  587, 554, 494, 370,                      // D5, C#5, B4, F#4
  
  // Pasito a pasito, suave suavecito
  370, 370, 370, 370, 494, 494, 494, 494,  // F#4s then B4s
  494, 440, 494, 392,                      // B4, A4, B4, G4
  
  // Poquito a poquito, suave suavecito
  392, 392, 392, 392, 494, 494, 494, 494,  // G4s then B4s
  494, 554, 587, 440                       // B4, C#5, D5, A4
};

int noteDurations[] = {
  // Des-pa-ci-to timing
  4, 8, 4, 2,                              
  
  // Pasito a pasito timing (Fast 16th notes)
  16, 16, 16, 16, 16, 16, 16, 16,          
  8, 8, 8, 2,                              
  
  // Poquito a poquito timing
  16, 16, 16, 16, 16, 16, 16, 16,          
  8, 8, 8, 2                               
};
int noteIndex = 0;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'>";
  html += "<title>Melody Buzzer</title>";
  html += "<style>body{font-family: Arial; text-align: center; margin-top: 50px;}</style></head><body>";
  html += "<h1>IoT Doorbell</h1>";
  html += "<h2>Touch Status: " + touchStatus + "</h2>";
  html += "<br><a href=\"/play\"><button style=\"font-size:24px; padding:15px 30px;\">RING DOORBELL</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

bool webTrigger = false;

void handlePlay() {
  webTrigger = true;
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/play", handlePlay);
  server.begin();
}

void playCurrentNote() {
  if (noteIndex >= sizeof(melody) / sizeof(int)) {
    noteIndex = 0;  // Restart riff
    return;
  }

  int duration = 1290 / noteDurations[noteIndex];
  tone(buzzerPin, melody[noteIndex]);
  
  // Custom delay allowing handleClient to run
  unsigned long start = millis();
  while(millis() - start < duration) {
    server.handleClient();
    delay(1);
  }
  noTone(buzzerPin);
  start = millis();
  while(millis() - start < 30) {
    server.handleClient();
    delay(1);
  }
  
  noteIndex++;
}

void loop() {
  server.handleClient();

  int touchValue = analogRead(touchSensorPin);
  
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.println(touchValue);
    lastPrint = millis();
  }

  if (touchValue < threshold || webTrigger) {
    touchStatus = (webTrigger) ? "WEB TRIGGERED (PLAYING)" : "TOUCHED (PLAYING)";
    playCurrentNote();
    if (noteIndex == 0) {
      webTrigger = false; // Stop when song finishes
    }
  } else {
    touchStatus = "WAITING";
    noTone(buzzerPin);  // Silence if not touching
  }
}
