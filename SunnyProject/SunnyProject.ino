/*
Program Description - sequential code written in order to avoid the obstacle using one sensor.
Its Not the real time avoidance system.
Move > Stop > Look 
*/
#include "AFMotor.h"
#include "NewPing.h"

#define TRIG_PIN A1 
#define ECHO_PIN A0

#define MAX_DISTANCE 400 
#define MAX_SPEED 190 // sets speed of DC  motors
#define MAX_SPEED_OFFSET 20

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

AF_DCMotor motorRight(1, MOTOR12_1KHZ); 
AF_DCMotor motorLeft(2, MOTOR12_1KHZ);

//AF_DCMotor motorRight(1, MOTOR34_1KHZ);
//AF_DCMotor motorLeft(4, MOTOR34_1KHZ);

boolean goesForward=false;
int distance = 100;
int speedSet = 0;
int randomNumber = 0;

void setup() {
  Serial.begin(9600);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
}

void loop() {
//    Serial.print("Distance = ");
//    Serial.print(distance);
//    Serial.println("cm");

    // Look for obstacle
    distance = readPing();
    delay(200);
 
    // stop the car
    moveStop();
    delay(200);
    
    if (distance > 0 && distance < 50) {
      moveBackward();
      delay(200);
      randomNumber = rand() % 10;
      if (randomNumber > 5) {
        turnRight();      
      } else {
        turnLeft();
      }
      
      Serial.print("Move back the car");
    } else {
      moveForward();
      if (distance > 100) {
          delay(300);
      } else {
          delay(200);
      }

      Serial.print("Move forward the car");
    }
    
    moveStop();
    delay(200);
}

int readPing() { 
  delay(70);
  int cm = sonar.ping_cm();

  if (cm == 0) {
    cm = 250;
  }
 
  return cm;
}

void moveStop() {
  motorRight.run(RELEASE); 
  motorLeft.run(RELEASE);
} 
  
void moveForward() {
  motorRight.run(FORWARD);
  motorLeft.run(FORWARD);

  // slowly bring the speed up to avoid loading down the batteries too quickly
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) {
    motorRight.setSpeed(speedSet);
    motorLeft.setSpeed(speedSet);
    delay(5);
  }
}

void moveBackward() {
  motorRight.run(BACKWARD);
  motorLeft.run(BACKWARD);

  // slowly bring the speed up to avoid loading down the batteries too quickly
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) {
    motorRight.setSpeed(speedSet);
    motorLeft.setSpeed(speedSet);
    delay(5);
  }
}

void turnRight () {
  motorRight.run(FORWARD);
  motorLeft.run(BACKWARD);
  delay(500);
} 

void turnLeft () {
  motorRight.run(BACKWARD);     
  motorLeft.run(FORWARD);
  delay(500);
}
