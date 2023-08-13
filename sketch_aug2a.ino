#include <ezButton.h>
#define PRESS LOW
#define RELEASE HIGH
 
int IN3 = 13;
int IN4 = 15;
ezButton limitSwitch_right(14);
ezButton limitSwitch_left(12);

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
  delay(1000);
  while(right == RELEASE){
    right = getButtonState(true);
    Serial.println("Right");
  }

  notMove();
  Serial.println("Stop 3s");
  delay(3000);
  Serial.println("After stop");

  spinLeft();
  int left = getButtonState(false);
  Serial.println(left);
  delay(1000);
  while(left == RELEASE){
    left = getButtonState(false);
    Serial.println("Left");
  }
  notMove();
}

void openingDoor(){
  spinRight();
  int right = getButtonState(true);
  while(right == RELEASE)
    right = getButtonState(true);
  notMove();
}

void closingDoor(){
  spinLeft();
  int left = getButtonState(false);
  while(left == RELEASE)
    left = getButtonState(false);
  notMove();
}

void setup() {
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  pinMode(2, INPUT);
  Serial.begin(9600);
  limitSwitch_right.setDebounceTime(50);
  limitSwitch_left.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  /*Serial.println("new loop");
  delay(1000);
  openAutomatic();
  Serial.println("10s");
  delay(10000);*/

  //if open
  openingDoor();
  //if close
  //closingDoor();
  delay(5000);
}
