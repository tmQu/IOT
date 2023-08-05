
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <PubSubClient.h>

//wifi
const char* ssid = "Wokwi-GUEST";
const char* password = "";


#define RST_PIN 16  // Configurable, see typical pin layout above
#define SS_PIN 15  // Configurable, see typical pin layout above

#define MASTER_CARD 1
#define EMPLOYEE_CARD 0
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

byte defaultKey[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

//
char passwordOpenDoor[34] = "211274072112738221127335";

String tagID;
int count = 0;
int timeForCounting = 2000;


struct PersonInformation {
  uint8 OrdinalNumber;
  char id[9];
  char name[34];
  char role;
  PersonInformation(char *inputId, char *inputName, char inputRole)
  {
    strcpy(id, inputId);
    strcpy(name, inputName);
    role = inputRole;
  }
};


void writeToBlock(byte buffer[], byte block)
{
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++)
    key.keyByte[i] = defaultKey[i];

  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
}

bool writeInformation(PersonInformation person) {


  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  Serial.print(F("Writing card UID:"));  //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  byte buffer[48];
  byte block;

  //---------------------------------------------------------
  //writing to sector 1 password
  for (int i = 0; i < strlen(passwordOpenDoor); i++)
    buffer[i] = (byte)passwordOpenDoor[i];

  buffer[strlen(passwordOpenDoor)] = (byte)person.role;
  
  writeToBlock(buffer, 1);
  writeToBlock(buffer + 16, 2);

  //---------------------------------------------
  // writing name to card
  for (int i = 0; i < strlen(person.name); i++)
      buffer[i] = (byte)person.name[i];
  writeToBlock(buffer, 4);
  writeToBlock(buffer + 16, 5);


  for (int i = 0; i < strlen(person.id); i++)
    buffer[i] = (byte)person.id[i];
  writeToBlock(buffer, 6);
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  return true;
}


//return -1 not valid card
// return 1 master card
// return 0 employee card
int checkPasswordOpenDoor()
{
  byte buffer[34];
  byte len = 18;

  MFRC522::StatusCode status;
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return -1;
  }

  status = mfrc522.MIFARE_Read(block, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  
    return -1;
  }

  block = 2;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    
    return -1;
  }

  status = mfrc522.MIFARE_Read(block, buffer + 16, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    
    return -1;
  }

  Serial.println(F("\n**End Reading**\n"));  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  int i = 0;
  for (i = 0; i < strlen(passwordOpenDoor); i++)
  {
    if (buffer[i] != (byte) passwordOpenDoor[i])
      return -1;
  }

  if (buffer[i] == (byte)'M')
    return MASTER_CARD;
    

  return EMPLOYEE_CARD;
}



//MQTT Receiver
PersonInformation parseMsg(char *msg, unsigned int length)
{


  return PersonInformation(id, name, role);
}
void callback(char* topic, byte* message, unsigned int length) {
  Serial.println(topic);
  if (strcmp(topic, "21127407/Register_NewCard")
  {
    //Parse msg
    char msg = (char*)message;
    char *id = strtok (msg + 2, "\",\"");
    char *name = strtok(NULL, "\",\"");
    char role = strtok(NULL, "\",\"")[0];

    PersonInformation person(id, name, role);
    long startWait = millis();
    while(startWait - millis() > 5000)
    {
      if (WriteInformation(person) == true);
      {
        Serial.println("write successfully");
      }
      else
      {
        Serial.println("write unsuccessfully");
      }
  
    }
  }

  }
  String strMsg;

  Serial.println(strMsg);

  //***Code here to process the received package***

}

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
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    Serial.print(clientId);
    if(mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");

      //***Subscribe all topic you need***
     
    }
    else {
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial)
    ;                                 // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522 card
                                      // put your setup code here, to run once:
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  wifiConnect();
  mqttClient.setServer(mqttServer, port);
  mqttClient.setCallback(callback);
  mqttClient.setKeepAlive( 90 );
}


void loop() {
  if(!mqttClient.connected()) {
    mqttConnect();
  }
  mqttClient.loop();

  // put your main code here, to run repeatedly:
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }


  int card = checkPasswordOpenDoor();

  // Invalid card
  if (card == -1)
    return;

  //mfrc522.PICC_HaltA();  // Stop reading
  
  unsigned long startCounting = millis();
  while (millis() - startCounting <= timeForCounting) {
    if (!mfrc522.PICC_IsNewCardPresent()) {
      continue;
    }
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
      continue;
    }

    for (int i = 0; i < 10; i++)
      if (uid[i] != mfrc522.uid.uidByte[i]) {
        count = 0;
        return;
      }
    count++;
    mfrc522.PICC_HaltA();  // Stop reading
    startCounting = millis();
  }


  Serial.println(count);
  if (card == EMPLOYEE_CARD)
  {
    if (count == 1)
    {
      mqttClient.publish("21127407/IOT_Smart_Door/CheckOpenDoor", buffer);
      return;
    }
    else if (count >= 3)
    {
      mqttClient.publish("21127407/IOT_Smart_Door/Req_Enter", buffer);
    }
  }
  else if (card == MASTER_CARD)
  {
    if (count == 1) // register a new card
    {
      mqttClient.publish("21127407/IOT_Smart_Door/Register_NewCard");
    }
    else (count >= 2)
    {
      mqttClient.publish("21127407/IOT_Smart_Door/OpenDoor", "True");
    }
  }
  


}
