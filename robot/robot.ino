#include <SoftwareSerial.h>
#define IN1_back 2
#define IN2_back 3
#define IN3_back 4
#define IN4_back 5

#define IN1_front 7
#define IN2_front 6
#define IN3_front 8
#define IN4_front 9

#define ir1 A1
#define ir2 A2
#define ir3 A3
#define ir4 A4
#define ir5 A5
#define ir0 A0

#define buzzer 10

#define BLACK_THRESHOLD

#define RUN_INTERVAL 25
#define TURN_INTERVAL 500
#define FORWARD_INTERVAL 200

#define strongForce 140
#define lightForce 120

#define idle 0
#define moving_to_room 1
#define moving_to_hall 2
#define waiting 3
#define moving_to_garage 4

#define None 0
#define LEFT 1
#define RIGHT 2

int state = idle;
int timer = 0;
int lastTurn = None;
int numberOfOrders = 0;
int numberOfOrdersLeft = 0;
int countTurns = 0;

boolean ir[5] = {0, 0, 0, 0, 0};
int rooms[7] = {1, 3, 6, 4, 5, 6, 7};
int turnsForEachRoom[7] = {0, 0, 0, 0, 0, 0, 0};
boolean isStop = false, isRoom = false, isTurn = false;

#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0

SoftwareSerial mySerial(0, 1);

void setup()
{
  pinMode(IN1_back, OUTPUT);
  pinMode(IN2_back, OUTPUT);
  pinMode(IN3_back, OUTPUT);
  pinMode(IN4_back, OUTPUT);
  
  pinMode(IN1_front, OUTPUT);
  pinMode(IN2_front, OUTPUT);
  pinMode(IN3_front, OUTPUT);
  pinMode(IN4_front, OUTPUT);
  
  pinMode(buzzer, OUTPUT);
  
  pinMode(ir0, INPUT);
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(ir5, INPUT);
  
  numberOfOrders = 3;
  numberOfOrdersLeft = 0;
  mySerial.begin(115200);
}

void motor_1_Stop() {
  digitalWrite(IN1_front, LOW);
  digitalWrite(IN2_front, LOW);
}

void motor_2_Stop() {
  digitalWrite(IN3_front, LOW);
  digitalWrite(IN4_front, LOW);
}
 
void motor_3_Stop() {
  digitalWrite(IN1_back, LOW);
  digitalWrite(IN2_back, LOW);
}
 
void motor_4_Stop() {
  digitalWrite(IN3_back, LOW);
  digitalWrite(IN4_back, LOW);
}

void motor_1_Forward(int speed) { //speed: từ 0 - MAX_SPEED
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
  digitalWrite(IN1_front, HIGH);
  analogWrite(IN2_front, 255 - speed);
}
 
void motor_1_Backward(int speed) {
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
  digitalWrite(IN1_front, LOW);
  analogWrite(IN2_front, speed);
}
 
void motor_2_Forward(int speed) { //speed: từ 0 - MAX_SPEED
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
  digitalWrite(IN3_front, HIGH);
  analogWrite(IN4_front, 255 - speed);
}
 
void motor_2_Backward(int speed) {
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
  digitalWrite(IN3_front, LOW);
  analogWrite(IN4_front, speed);
}

void motor_3_Forward(int speed) { //speed: từ 0 - MAX_SPEED
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
  digitalWrite(IN1_back, HIGH);
  analogWrite(IN2_back, 255 - speed);
}
 
void motor_3_Backward(int speed) {
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
  digitalWrite(IN1_back, LOW);
  analogWrite(IN2_back, speed);
}
 
void motor_4_Forward(int speed) { //speed: từ 0 - MAX_SPEED
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
  digitalWrite(IN3_back, HIGH);
  analogWrite(IN4_back, 255 - speed);
}
 
void motor_4_Backward(int speed) {
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
  digitalWrite(IN3_back, LOW);
  analogWrite(IN4_back, speed);
}

void robotForward(int speed1, int speed2 ,int speed3, int speed4)
{
  motor_1_Forward(speed1);
  motor_2_Forward(speed2);
  motor_3_Forward(speed3);
  motor_4_Forward(speed4);
}

void robotBackward(int speed1, int speed2 ,int speed3, int speed4)
{
  motor_1_Backward(speed1);
  motor_2_Backward(speed2);
  motor_3_Backward(speed3);
  motor_4_Backward(speed4);
}

void robotLeft(int speed1, int speed2 ,int speed3, int speed4)
{
  motor_1_Backward(speed1);
  motor_2_Forward(speed2);
  motor_3_Forward(speed3);
  motor_4_Backward(speed4);
}

void robotRight(int speed1, int speed2 ,int speed3, int speed4)
{
  motor_1_Forward(speed1);
  motor_2_Backward(speed2);
  motor_3_Backward(speed3);
  motor_4_Forward(speed4);
}

void robotTurnLeft(int speed1, int speed2, int speed3, int speed4) 
{
  motor_1_Forward(speed1);
  motor_2_Backward(speed2);
  motor_3_Forward(speed3);
  motor_4_Backward(speed4);
}

void robotTurnRight(int speed1, int speed2, int speed3, int speed4) 
{
  motor_1_Backward(speed1);
  motor_2_Forward(speed2);
  motor_3_Backward(speed3);
  motor_4_Forward(speed4);
}

void frontLeft(int speed1, int speed2, int speed3, int speed4)
{
  motor_1_Forward(speed1);
  motor_2_Stop();
  motor_3_Stop();
  motor_4_Forward(speed4);
}

void frontRight(int speed1, int speed2, int speed3, int speed4)
{
  motor_1_Stop();
  motor_2_Forward(speed2);
  motor_3_Forward(speed3);
  motor_4_Stop();
}

void robotStop() {
  motor_1_Stop();
  motor_2_Stop();
  motor_3_Stop();
  motor_4_Stop();
}

void robotTurnRight_90_degree(int force) {
  //robotStop();
  //delay(1000);
  lastTurn = RIGHT;
  while (!ir[0]) {
    robotTurnRight(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
  while (ir[0]) {
    robotForward(force, force, force, force);
    readIR();
  }
  robotStop();
  while (!ir[1]) { 
    robotTurnRight(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
}

void robotTurnLeft_90_degree(int force) {
  //robotStop();
  //delay(1000);
  lastTurn = LEFT;
  while (!ir[4]) {
    robotTurnLeft(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
  while (ir[4]) {
    robotForward(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
  while (!ir[3]) { 
    robotTurnLeft(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
}

void robotTurnLeft_90_degree_to_hall(int force) {
  
  while (ir[0] | ir[4]) {
    robotForward(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
  while (!ir[4]) {
    robotTurnLeft(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
  while (!ir[2]) {
    frontRight(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
}

void robotTurnRight_90_degree_to_hall(int force) {
  
  while (ir[0] | ir[4]) {
    robotForward(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
  while (!ir[0]) {
    robotTurnRight(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
  while (!ir[2]) {
    frontLeft(force, force, force, force);
    readIR();
    if (isStop) {
      robotStop();
      //return;
    }
  }
  robotStop();
}

void readIR() {
  ir[0] = !(digitalRead(ir1));
  ir[1] = !(digitalRead(ir2));
  ir[2] = !(digitalRead(ir3));
  ir[3] = !(digitalRead(ir4));
  ir[4] = !(digitalRead(ir5));
  isStop = digitalRead(ir0);
  isRoom = ir[0] & ir[4] & ir[1] & ir[2] & ir[3] & isTurn;
}

void robotMovingToRoom() {
  //int strongForce = 120; //100 for full batteries
  //int lightForce = 100; //80 for full batteries
  if (ir[4] & (!isTurn)) {
    robotStop();
    delay(1000);
    countTurns += 1;
    int idx  = numberOfOrders - numberOfOrdersLeft;
    if (countTurns == turnsForEachRoom[idx]) {
      countTurns = 0;
      robotTurnRight_90_degree(lightForce);
      isTurn = true;
    } else {
      while (ir[4]) {
        robotForward(strongForce, strongForce, strongForce, strongForce);
        readIR();
      }
    }
  } else if (ir[0] & (!isTurn)) {
    robotStop();
    delay(1000);
    countTurns += 1;
    int idx  = numberOfOrders - numberOfOrdersLeft;
    if (countTurns == turnsForEachRoom[idx]) {
      countTurns = 0;
      robotTurnLeft_90_degree(lightForce);
      isTurn = true;
    } else {
      while (ir[0]) {
        robotForward(strongForce, strongForce, strongForce, strongForce);
        readIR();
      }
    }
  } else if (ir[2] & (!ir[1]) & (!ir[3])) {
    robotForward(strongForce, strongForce, strongForce, strongForce);
  } else if ((!ir[2]) & (!ir[1]) & (!ir[3])) {
    robotForward(lightForce, lightForce, lightForce, lightForce);
  } else if (ir[1]) {
    robotTurnLeft(lightForce, lightForce, lightForce, lightForce);
  } else if (ir[3]) {
    robotTurnRight(lightForce, lightForce, lightForce, lightForce);
  }
}

void robotMovingToHall() {
  //int strongForce = 120; //100 for full batteries
  //int lightForce = 100; //80 for full batteries
  if (ir[2] & (!ir[1]) & (!ir[3])) {
    robotForward(lightForce, lightForce, lightForce, lightForce);
  } else if ((!ir[2]) & (!ir[1]) & (!ir[3])) {
    robotForward(lightForce, lightForce, lightForce, lightForce);
  } else if (ir[1]) {
    robotTurnLeft(lightForce, lightForce, lightForce, lightForce);
  } else if (ir[3]) {
    robotTurnRight(lightForce, lightForce, lightForce, lightForce);
  }
}

void robotMovingToGarage() {
  //int strongForce = 120; //100 for full batteries
  //int lightForce = 100; //80 for full batteries
  if (ir[2] & (!ir[1]) & (!ir[3])) {
    robotForward(strongForce, strongForce, strongForce, strongForce);
  } else if (ir[1] & ir[0]) {
    robotForward(lightForce, lightForce, lightForce, lightForce);
  } else if (ir[3] & ir[4]) {
    robotForward(lightForce, lightForce, lightForce, lightForce);
  } else if (ir[1]) {
    robotTurnLeft(lightForce, lightForce, lightForce, lightForce);
  } else if (ir[3]) {
    robotTurnRight(lightForce, lightForce, lightForce, lightForce);
  }
}

void replyToCommand() {
  mySerial.print("!0#");
}

void reportArrival() {
  mySerial.print("!u#");
}

void reportDestination(String room) {
  mySerial.print("!" + room + "#");
}

void alarmBuzzer(int duration) {
  digitalWrite(buzzer, HIGH);
  delay(duration);
  digitalWrite(buzzer, LOW);
  delay(duration);
}


void readOrders() {
  String data = "";
  char c = char(mySerial.read());
  if (c == '!') {
    while (mySerial.available()) {
      c = char(mySerial.read());
      if (c != '#') {
        data.concat(c);
      } else {
        data.concat(':');
        String temp = "";
        int pos = 0;
        int count = 0;
        for (int i = 0; i < data.length(); i++) {
          c = data.charAt(i);
          if (c != ':') {
            temp.concat(c);
          } else {
            int number = temp.toInt();
            if (count == 0) {
              numberOfOrders = number;
              numberOfOrdersLeft = numberOfOrders;
            } else {
              rooms[numberOfOrders - count] = number;
            }
            pos = i + 1;
            count += 1;
            temp = "";
          }
        }
      }
    }
    //replyToCommand();
    //Serial.print(data);
  }
}

void runn() {
  int force = strongForce;
  switch (state) {
    case idle:
      robotStop();
      readIR();
      //if (mySerial.available()) readOrders();
      if (isStop) { //numberOfOrdersLeft != 0
        numberOfOrdersLeft = numberOfOrders;
        turnsForEachRoom[0] = rooms[0];
        for (int i = 1; i < numberOfOrders; i++) turnsForEachRoom[i] = rooms[i] - rooms[i-1];
        rooms[numberOfOrders] = 0;
        countTurns = 0;
        isTurn = false;
        delay(1000);
        //reportDestination(String(rooms[0]));
        alarmBuzzer(500);
        state = moving_to_room;
      }
      break;
    case moving_to_room:
      readIR();
      if (isStop) {
        robotStop();
        alarmBuzzer(100);
      } else if (!isRoom) {
        robotMovingToRoom();
      } else {
        robotStop();
        reportArrival();
        numberOfOrdersLeft -= 1; //new
        alarmBuzzer(200);
        alarmBuzzer(200);
        alarmBuzzer(200);
        state = waiting;
      }
      break;
    case waiting: 
      delay(100);
      timer++;
      if (timer == 20) {
        timer = 0;
        isTurn = false;
        while (ir[0] | ir[4]) {
          readIR();
          if (isStop) {
            robotStop();
          } else robotForward(force, force, force, force);
        }
        robotStop();
        while (!ir[3]) {
          robotTurnLeft(force, force, force, force);
          readIR();
        }
        robotStop();
        while (ir[3]) {
          robotTurnLeft(force, force, force, force);
          readIR();
        }
        robotStop();   
        while (!ir[3]) {
          robotTurnLeft(force - 10, force - 10, force - 10, force - 10);
          readIR();
        }
        robotStop();    
        state = moving_to_hall;
      } else {
        if (timer == 190) reportDestination(String(rooms[numberOfOrders - numberOfOrdersLeft])); //new
        state = waiting;
      }
      break;
    case moving_to_hall:
      readIR();
      if (isStop) {
        robotStop();
        alarmBuzzer(100);
      } else if (ir[0] | ir[4]) {
        robotStop();
        delay(1000);
        if (numberOfOrdersLeft > 0) {
          if (lastTurn == LEFT) {
            robotTurnLeft_90_degree_to_hall(force);
          } else if (lastTurn == RIGHT) {
            robotTurnRight_90_degree_to_hall(force);
          }
          //numberOfOrdersLeft -= 1;
          state = moving_to_room;
        } else {
          if (lastTurn == RIGHT) {
            robotTurnLeft_90_degree_to_hall(force);
          } else if (lastTurn == LEFT) {
            robotTurnRight_90_degree_to_hall(force);
          }
          //numberOfOrdersLeft -= 1;
          isTurn = true;
          state = moving_to_garage;
        } 
      } else {
        robotMovingToHall();
        state = moving_to_hall;
      }
      break;
    case moving_to_garage:
      readIR();
      if (isStop) {
        robotStop();
        alarmBuzzer(100);
      } else if (isRoom) {
        robotStop();
        delay(1000);
        while (ir[0] | ir[4]) {
          readIR();
          if (isStop) {
            robotStop();
          } else robotForward(force, force, force, force);
        }
        robotStop();
        while (!ir[3]) {
          robotTurnLeft(force, force, force, force);
          readIR();
        }
        robotStop();
        while (ir[3]) {
          robotTurnLeft(force, force, force, force);
          readIR();
        }
        robotStop();   
        while (!ir[2]) {
          robotTurnLeft(force - 10, force - 10, force - 10, force - 10);
          readIR();
        }
        robotStop();
        //replyToCommand();
        reportArrival();
        alarmBuzzer(1000);
        state = idle;
      } else {
        robotMovingToGarage();
      }
      break;
  }
}
 
void loop() {
  runn();
}
