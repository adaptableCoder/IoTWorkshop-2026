#include <DHT.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NSUT_WIFI";
const char* password = "";

WebServer server(80);

#define DHTPIN 4          // GPIO 4 for DHT11 data
#define DHTTYPE DHT11     
#define SERVOPIN 18       // GPIO 18 for Servo signal

DHT dht(DHTPIN, DHTTYPE);
Servo fanServo;

const float TEMP_MIN = 25.0;   
const float TEMP_MAX = 35.0;   
const int MAX_DELAY = 45;      
const int MIN_DELAY = 8;       
const int STEP_SIZE = 5;       

float currentTemp = 0.0;
String fanStatus = "IDLE";
String warningMsg = "";

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='2'>";
  html += "<title>Smart Fan</title>";
  html += "<style>body{font-family: Arial; text-align: center; margin-top: 50px;}</style></head><body>";
  html += "<h1>Temperature Controlled Fan</h1>";
  html += "<h2>Current Temp: " + String(currentTemp, 1) + " &deg;C</h2>";
  html += "<h2>Fan Status: " + fanStatus + "</h2>";
  if (warningMsg != "") {
    html += "<h2 style='color:red;'>" + warningMsg + "</h2>";
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200); 
  dht.begin();
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  fanServo.setPeriodHertz(50); 
  fanServo.attach(SERVOPIN, 500, 2400); 
  fanServo.write(0); 

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

  float temp = dht.readTemperature();
  if (isnan(temp)) {
    // delay(2000); // Replacing blocking delay
    unsigned long start = millis();
    while(millis() - start < 2000) { server.handleClient(); delay(10); }
    return; 
  }
  currentTemp = temp;

  if (temp >= TEMP_MAX) {
    warningMsg = "WARNING: HIGH TEMPERATURE";
  } else if (temp >= TEMP_MIN) {
    warningMsg = "Fan is running to cool down.";
  } else {
    warningMsg = "";
  }

  if (temp >= TEMP_MIN) {
    fanStatus = "ON";
    float tempPercentage = (temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
    if (tempPercentage > 1.0) tempPercentage = 1.0; 
    int sweepDelay = MAX_DELAY - (tempPercentage * (MAX_DELAY - MIN_DELAY));

    float totalCycleTimeMs = (360 / STEP_SIZE) * 2 * sweepDelay;
    float estimatedRPM = 60000.0 / totalCycleTimeMs;

    Serial.print("Temp: ");
    Serial.print(temp, 1);
    Serial.print(" °C | Step Delay: ");
    Serial.print(sweepDelay);
    Serial.print("ms | Fan Speed: ");
    Serial.print(estimatedRPM, 1);
    Serial.println(" RPM");

    for (int angle = 0; angle <= 180; angle += STEP_SIZE) {
      fanServo.write(angle);
      unsigned long start = millis();
      while(millis() - start < sweepDelay) { server.handleClient(); delay(1); }
    }
    for (int angle = 180; angle >= 0; angle -= STEP_SIZE) {
      fanServo.write(angle);
      unsigned long start = millis();
      while(millis() - start < sweepDelay) { server.handleClient(); delay(1); }
    }
  } else {
    fanServo.write(0);
    fanStatus = "IDLE";
    float estimatedRPM = 0.0;

    Serial.print("Temp: ");
    Serial.print(temp, 1);
    Serial.print(" °C | Fan Status: IDLE | Speed: ");
    Serial.print(estimatedRPM, 1);
    Serial.println(" RPM");

    for(int i=0; i<20; i++){ server.handleClient(); delay(100); }
  }
}