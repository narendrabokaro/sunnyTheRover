/*
 * Program name - Sunny - the curiosity bot [Rewrite using L298N motor driver]
 * Description - Full fledge autonomous bot which can avoid the obstacle and take shortest path possible
 * Rewritten program - Using small motor driver - L298N and some more enhancements
 * Work in progress - 
 * 
 * TARGETS -
 *    1) first run both motor forward and then backward for 2 seconds with PWM settings [Done]
 *    2) do the same things with PWM Setting so that pickup will be little smooth [Done]
 *    3) make a square = forward > change to right (45 degree) direction & then move forward > change to right (45 degree) direction & then move forward > change to right (45 degree) direction & then move forward
 *    4) Reached to level of Sunny v2 [DONE]
 *    5) Integrate two extra ultrasonic sensor (L&R) to more intelligent directional descion makings
 *    6) Integrate MPU6050 sensor for obstacle or roll based descion makings
 *    7) Includes lights for reverse gears
*/

#include "NewPing.h"

// Center Sonar pins
#define TRIG_PIN A4
#define ECHO_PIN A5

#define MAX_DISTANCE 400
// sets speed of DC  motors
#define MAX_SPEED 190
// Delay value for forward motor movement
#define MOTORDELAY 3
// Delay value for  backward motor movement
#define MOTORBACKWARDDELAY 1
// Delay for motor to 45 degree turns
#define TURNDELAY 75

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

int distance = 0;

// Motor A connections
int enA = 9;
int in1 = 2;
int in2 = 3;

// Motor B connections
int enB = 10;
int in3 = 4;
int in4 = 5;

int rightInfraredSensor = 13;  // This is our input pin
int leftInfraredSensor = 12;  // This is our input pin

void setup() {
  // Set infrared sensors
  pinMode(rightInfraredSensor, INPUT);
  pinMode(leftInfraredSensor, INPUT);

  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // Sonar initialization begin here
  Serial.begin(9600);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
}

void loop() {
   // Look for obstacle
    distance = readPing();

    Serial.print("Distance: ");
    Serial.println(distance);
    
    if (distance > 0 && distance < 50) {
        // stop, reverse and change the direction
        stopMotors();
        moveBackward();
        takeRightTurn();
        stopMotors();
        Serial.println("Move back the car");
    } else {
        moveFoward();
        Serial.println("Move forward the car");
    }

    delay(200);
}

int checkRightInfraredSensor() {
    int isFound = 0;
    if (digitalRead(rightInfraredSensor) == LOW) {
      Serial.println("OBSTACLE!!, OBSTACLE!!");
      isFound = 1;
    } else {
      Serial.println("No OBSTACLE found");
      isFound = 0;
    }
    return isFound;
}

int checkLeftInfraredSensor() {
    int isFound = 0;
    if (digitalRead(leftInfraredSensor) == LOW) {
      Serial.println("OBSTACLE!!, OBSTACLE!!");
      isFound = 1;
    } else {
      Serial.println("No OBSTACLE found");
      isFound = 0;
    }
    return isFound;
}

int readPing() {
  // Wait till 50ms before sending a sonar wave 
  delay(50);
  int cm = sonar.ping_cm();

  if (cm == 0) {
    cm = 250;
  }
 
  return cm;
}

void takeRightTurn() {
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  delay(TURNDELAY);
}

void takeLeftTurn() {
  // Turn off motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);  

  delay(TURNDELAY);
}

void stopMotors() {
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void moveBackward() {
  // Turn on motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  // Accelerate from zero to maximum speed
  for (int i = 0; i < MAX_SPEED; i++) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(MOTORBACKWARDDELAY);
  }
  
//  // Decelerate from maximum speed to zero
//  for (int i = MAX_SPEED; i >= 0; --i) {
//    analogWrite(enA, i);
//    analogWrite(enB, i);
//    delay(5);
//  }
    // Turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void moveFoward() {
  // Turn on motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
  // Accelerate from zero to maximum speed
  for (int i = 0; i < MAX_SPEED; i++) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(MOTORDELAY);
  }
  
//  // Decelerate from maximum speed to zero
//  for (int i = MAX_SPEED; i >= 0; --i) {
//    analogWrite(enA, i);
//    analogWrite(enB, i);
//    delay(5);
//  }

    // Turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}
