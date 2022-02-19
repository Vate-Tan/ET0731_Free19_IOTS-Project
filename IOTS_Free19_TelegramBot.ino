//----------INCLUDE LIBRARIES---------
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  
#include <ArduinoJson.h>
#include <Servo.h>

//----------DEFINE VARIABLES---------
const int RLED = D0;
const int Buzzer = D7;

// Replace with your network credentials
const char* ssid = "*******";
const char* password = "*********";

// Initialize Telegram BOT token
#define BOTtoken "************************"

// Initialize Telegram Personal ID
#define CHAT_ID "**********"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);

Servo servo;
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// Handle new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "You don't have access to Free19_Bot, Unauthorized User Detected.", ""); //Unauthorise User Entering
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome to Free19_Bot, " + from_name + ".\n\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/alert to alarm for intruders \n";
      welcome += "/opengate to open gantry gate \n";
      welcome += "/closegate to close gantry gate \n";
      welcome += "/state to request current gantry gate state \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/alert") {
      bot.sendMessage(chat_id, "Alerted", "");
      digitalWrite(RLED,HIGH);
      delay(100);
      digitalWrite(RLED,LOW);
      tone(Buzzer, 2000, 500);
    }

    if (text == "/opengate") {
      bot.sendMessage(chat_id, "Gantry gate opened", "");
      servo.write(0);
      delay(100);
    }
    
    if (text == "/closegate") {
      bot.sendMessage(chat_id, "Gantry gate closed", "");
      servo.write(180);
      delay(100);
    }
    
    if (text == "/state") {
      if (servo.read()==180){
        bot.sendMessage(chat_id, "Gantry gate closed", "");
      }
      else{
        bot.sendMessage(chat_id, "Gantry gate opened", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RLED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  servo.attach(D4);
  servo.write(0);
  delay(1000);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert);        // Add root certificate for api.telegram.org
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
