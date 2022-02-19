//-----------INCLUDE LIBRARIES---------//
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
//-------------------------------------------//
const int LM35 = A0;
const int GLED = D0;
const int RLED = D1;
const int trigPin = D6;
const int echoPin = D5;
const int Buzzer = D7;
int ADC = 0;
int vacancy = 200;
float temp = 0;
long duration = 0;
float distance = 0;
int gantry = 0;

#define SOUND_VELOCITY 0.034

// ********** ThingSpeak Credentials ***********
unsigned long channelID = ********;
String writeAPIKey = "**************"; // Write API key for your ThingSpeak channel
const char* server = "http://api.thingspeak.com/update"; // API for ThingSpeak
const char* fingerprint = "27 18 92 DD A4 26 C3 07 09 B9 7A E6 C5 21 B9 5B 48 F7 16 E1";
// *********************************************
// Set up WiFi Connection
const char *ssid = "*********";
const char *pass = "************";

Servo servo;
WiFiClient client;

void setup() {
 // put your setup code here, to run once:
 Serial.begin(115200);
 delay(100);
 Serial.print("Trying to Connect with ");
 Serial.println(ssid);
 WiFi.begin(ssid,pass);
 while (WiFi.status() != WL_CONNECTED)
 {
 delay(500);
 Serial.print(".");
 }
 Serial.println(" ");
 Serial.println("WiFi connected");
 Serial.print("IP Address: ");
 Serial.println(WiFi.localIP());
 ThingSpeak.begin(client); // Initialize ThingSpeak
 
 pinMode(GLED, OUTPUT);
 pinMode(RLED, OUTPUT);
 pinMode(Buzzer, OUTPUT);
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
 pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 servo.attach(D4);
 servo.write(0);
 delay(1000);  
}
void writeData() {
  if(temp > 37){
    gantry = 0;
    digitalWrite(RLED,HIGH);
    digitalWrite(GLED,LOW);
    tone(Buzzer, 2000, 500);
    servo.write(180);
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  else{
    gantry = 1;
    digitalWrite(RLED,LOW);
    digitalWrite(GLED,HIGH);
    servo.write(0);
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  if(distance > 48 && vacancy <= 199){
    vacancy++;
    Serial.print("Vacancies = ");
    Serial.print(vacancy);
    Serial.print(" ");
  }
  if(distance <48){
    vacancy--;
    Serial.print("Vacancies = ");
    Serial.print(vacancy);
    Serial.print(" ");
  }
  else{
    Serial.print("Vacancies = ");
    Serial.print(vacancy);
    Serial.print(" ");
  }
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * SOUND_VELOCITY/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.println(" cm");
  ADC = analogRead(LM35);  /* Read Temperature */
  temp = ((ADC * 330.0)/1024.0);
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");
  delay(5000);
 


 // Write to the ThingSpeak channel
 if ((WiFi.status() == WL_CONNECTED)) 
 {
 HTTPClient http;
 Serial.print("[HTTPS] begin ... \n");
 http.begin(client ,server);
 http.addHeader("Content-Type","application/x-www-form-urlencoded");
 String httpRequestData = "api_key=" + writeAPIKey + "&field1=" + String(temp);
 httpRequestData += "&field2=";
 httpRequestData += String(gantry);
 httpRequestData += "&field3=";
 httpRequestData += String(vacancy);
 int httpCode = http.POST(httpRequestData);
 Serial.print("HTTP Response code: ");
 Serial.println(httpCode);
 if (httpCode == 200) {
 Serial.println("Channels update successful.");
 }
 else {
 Serial.println("Data upload failed .....");
 Serial.println("Problem updating channel. HTTP error code " +
String(httpCode));
 }
 delay(20000);
 }
}
void loop() {
  writeData();
}
