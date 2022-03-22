#include <AFMotor.h>  
#include <NewPing.h>

#define TRIG_PIN A0 
#define ECHO_PIN A1 

#define MAX_DISTANCE 200 
#define MAX_SPEED 190 // sets speed of DC  motors
#define MAX_SPEED_OFFSET 20

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 

AF_DCMotor motorRight(1, MOTOR12_1KHZ); 
AF_DCMotor motorLeft(4, MOTOR12_1KHZ);

//AF_DCMotor motorRight(1, MOTOR34_1KHZ);
//AF_DCMotor motorLeft(4, MOTOR34_1KHZ);

boolean goesForward=false;
int distance = 100;
int speedSet = 0;

void setup() {
  motorRight.setSpeed(50);                 //Set the motors to the motor speed
  motorLeft.setSpeed(50);
  
  motorRight.run(RELEASE);                         //Ensure all motors are stopped
  motorLeft.run(RELEASE);
}

void loop() { 
// if(distance<=15)
// {
//  moveStop();
//  delay(100);
//  moveBackward();
//  delay(300);
//  moveStop();
//  delay(200);
// }else
// {
//  moveForward();
// }
// 
// distance = readPing();

  moveForward();
  delay(500);
  moveStop();
  delay(100);
  moveBackward();
  delay(300);
}

int readPing() { 
  delay(70);
  int cm = sonar.ping_cm();
  if(cm==0)
  {
    cm = 250;
  }
  return cm;
}

void moveStop() {
  motorRight.run(RELEASE); 
  motorLeft.run(RELEASE);
  } 
  
void moveForward() {

 if(!goesForward)
  {
    goesForward=true;
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
      
   for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
   {
    motorRight.setSpeed(speedSet);
    motorLeft.setSpeed(speedSet);
    delay(5);
   }
  }
}

void moveBackward() {
    goesForward=false;
    motorRight.run(BACKWARD);      
    motorLeft.run(BACKWARD);

  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
  {
    motorRight.setSpeed(speedSet);
    motorLeft.setSpeed(speedSet);
    delay(5);
  }
}  
