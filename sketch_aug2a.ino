#include <ezButton.h>
  // create ezButton object that attach to ESP32 pin GPIO17
int IN1 = 5;
int IN2 = 4;
int prev_left;
int prev_right;
int closed_time = 0;
ezButton limitSwitch_right(14);
ezButton limitSwitch_left(12);

void spinLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void spinRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void notMove(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void openAutomatic(int left, int right){
  if(left == LOW || right == LOW){
    prev_left = left;
    prev_right = right;
    notMove();
  }
  else{
    if(prev_left == LOW && prev_right == HIGH){
      spinRight();
    }
    else if(prev_left == HIGH && prev_right == LOW){
      spinLeft();
    }
  }
  /*if(left == LOW){
    prev_left = left;
    prev_right = right;
    notMove();
  }
  else if(right == LOW){
    prev_left = left;
    prev_right = right;
    notMove();
    delay(3000);
    //spinLeft();
  }
  else{
    if(prev_left == LOW && prev_right == HIGH){
      spinRight();
    }
    else if(prev_left == HIGH && prev_right == LOW){
      spinLeft();
    }
  }*/
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  pinMode(2, INPUT);
  Serial.begin(9600);
  limitSwitch_right.setDebounceTime(50);
  limitSwitch_left.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  limitSwitch_right.loop(); // MUST call the loop() function first
  limitSwitch_left.loop();
  int right = limitSwitch_right.getState();
  int left = limitSwitch_left.getState();
  openAutomatic(left, right);
  //spinLeft();
}
