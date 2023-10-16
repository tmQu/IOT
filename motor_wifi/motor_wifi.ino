#include <ezButton.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#define PRESS LOW
#define RELEASE HIGH


int IN3 = 13;
int IN4 = 15;
ezButton limitSwitch_right(14);
ezButton limitSwitch_left(12);

int waitTime = 3000;



const char* ssid = "Penguin";
const char* password = "1234567890";

//***Set server***
const char* mqttServer = "broker.hivemq.com"; 
int port = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void mqttConnect() {
  while(!mqttClient.connected()) {
    Serial.println("Attemping MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");

      //***Subscribe all topic you need***
      mqttClient.subscribe("IOT_Smart_Door/Motor/OpenAuto");
      mqttClient.subscribe("IOT_Smart_Door/Motor/SetWaiting");
     
    }
    else {
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

//MQTT Receiver
void callback(char* topic, byte* message, unsigned int length) {
  Serial.println(topic);
  if(strcmp((char*)topic, "IOT_Smart_Door/Motor/OpenAuto") == 0)
  {

      openAutomatic();
  }
  else if (strcmp((char*)topic, "IOT_Smart_Door/Motor/SetWaiting") == 0)
  {
    String strMsg = "";
    for(int i=0; i<length; i++) {
      strMsg += (char)message[i];
    }
    waitTime = strMsg.toInt();
    Serial.print("waitime ");
    Serial.println(waitTime);
  }

  //***Code here to process the received package***

}


void spinLeft() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void spinRight() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void notMove(){
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

int getButtonState(bool rightBut)
{
  limitSwitch_right.loop();
  int right = limitSwitch_right.getState();
  limitSwitch_left.loop();
  int left = limitSwitch_left.getState();
  if (rightBut ==  true)
    return right;
  return left;
}

void openAutomatic(){
  spinRight();
  int right = getButtonState(true);
  Serial.println(right);
  delay(500);
  while(right == RELEASE){
    right = getButtonState(true);
    Serial.println("Right");
  }

  notMove();
  Serial.println("Stop");
  delay(waitTime);
  Serial.println("After stop");

  spinLeft();
  int left = getButtonState(false);
  delay(500);
  while(left == RELEASE){
    left = getButtonState(false);
    Serial.println("Left");
  }
  notMove();
}

void openingDoor(){
  spinRight();
  int right = getButtonState(true);

  delay(500);
  while(right == RELEASE)
    right = getButtonState(true);
  notMove();
}

void closingDoor(){
  spinLeft();
  int left = getButtonState(false);
  delay(500);
  while(left == RELEASE)
    left = getButtonState(false);
  notMove();
}

void setup() {
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  pinMode(2, INPUT);
  Serial.begin(9600);
  Serial.print("Connecting to WiFi");
  WiFiManager wm;
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("Door system","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }
  //wifiConnect();
  mqttClient.setServer(mqttServer, port);
  mqttClient.setCallback(callback);
  mqttClient.setKeepAlive( 90 );
  limitSwitch_right.setDebounceTime(50);
  limitSwitch_left.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  /*Serial.println("new loop");
  delay(1000);
  openAutomatic();
  Serial.println("10s");
  delay(10000);*/
    if(!mqttClient.connected()) {
    mqttConnect();
  }
  mqttClient.loop();

}