#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "DHT"
#define BLYNK_AUTH_TOKEN "BPOK3C1-FQCkzVlnF0oFhX0iZZuHCJvq"

// Print messages to Serial Monitor
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Your WiFi credentials
char ssid[] = "NSUT_WIFI";
char pass[] = "";

// DHT Sensor Setup
#define DHTPIN 4          // Digital pin connected to the DHT sensor (GPIO 4 / D4)
#define DHTTYPE DHT11     // Type of DHT sensor (DHT11, DHT22, etc.)
DHT dht(DHTPIN, DHTTYPE); // Starting the sensor through library

// Create a timer object
BlynkTimer timer;

// Function to read sensor and send data to Blynk
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  // Check if any reads failed and exit early (to try again)
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send data to Blynk Virtual Pins
  // Assuming V0 is set up for Temperature and V1 for Humidity in your Blynk Datastreams
  Blynk.virtualWrite(V0, t); 
  Blynk.virtualWrite(V1, h);

  // Print to Serial monitor for debugging
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(h);
  Serial.println(" %");
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize DHT sensor
  dht.begin();

  // Setup a function to be called every 2 seconds
  // DHT11 requires at least 2 seconds between readings
  timer.setInterval(2000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}