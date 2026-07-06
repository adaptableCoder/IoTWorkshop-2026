#include <ESP32Servo.h>

Servo myservo; 
int ECHO = 18;
int TRIG = 5;


void setup() {
  // put your setup code here, to run once:
pinMode(TRIG, OUTPUT); 
pinMode(ECHO, INPUT); 
myservo.attach(4);
Serial.begin(115200);
}

long getDistance() {
digitalWrite(TRIG, LOW);
delayMicroseconds(2);

digitalWrite(TRIG, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG, LOW); 

long duration = pulseIn(ECHO, HIGH);

long distance = duration * (0.034/2); 

return distance;
}

void loop() {
  // put your main code here, to run repeatedly:


myservo.write(0);
delay(500);
Serial.print("The angle now is 0 degrees and the object distance is:"); 
Serial.println(getDistance());
delay(2000);

myservo.write(90);
delay(500);
Serial.print("The angle now is 90 degrees and the object distance is:"); 
Serial.println(getDistance());
delay(2000);

myservo.write(180);
delay(500);
Serial.print("The angle now is 180 degrees and the object distance is:"); 
Serial.println(getDistance());
delay(2000);
}