//--------------INCLUDE LIBRARIES----------
#include "ThingsBoard.h"
#include <ESP8266WiFi.h>
//--------------DEFINE VARIABLES----------
const int LM35 = A0;
const int trigPin = D6;
const int echoPin = D5;
int ADC = 0;
float temp = 0;
int gantry = 0;
int vacancy = 200;
long duration = 0;
float distance = 0;
//--------------SSID & TOKEN----------
#define WIFI_AP             "*************"
#define WIFI_PASSWORD       "*************"
#define TOKEN               "**********************"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

#define SERIAL_DEBUG_BAUD   115200

#define SOUND_VELOCITY 0.034

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

void setup() 
{
  // initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  InitWiFi();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop() 
{
  delay(3000);
  
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }
  ADC = analogRead(LM35);  /* Read Temperature */
  temp = ((ADC * 330.0)/1024.0);
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * SOUND_VELOCITY/2;

  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");


  if(distance > 48 && vacancy <= 199){
    vacancy++;
    Serial.print("Vacancies = ");
    Serial.print(vacancy);
    Serial.print(" ");
  }
  if(distance < 48){
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

  if(temp > 37){
    gantry = 0;
    Serial.print("Gantry = ");
    Serial.print(gantry);
    Serial.print(" ");
  }
  else{
    gantry = 1; 
    Serial.print("Gantry = ");
    Serial.print(gantry);
    Serial.print(" ");
  }
  
  tb.sendTelemetryFloat("Temperature", temp);
  tb.sendTelemetryInt("Gantry Gate", gantry);
  tb.sendTelemetryInt("Vacancies", vacancy);
  tb.loop();
  delay(5000);
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
