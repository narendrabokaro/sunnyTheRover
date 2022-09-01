/*
 * Program name - Sunny - the curiosity bot [Rewrite using L298N motor driver]
 * Description - Full fledge autonomous bot which can avoid the obstacle and take shortest path possible
 * Whats new in this version - Included hr04 sonar sensor as interrupt. Means achieved the capability of "Move forward while looking"
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

// New interrupts for ultrasonic sensor
#include <TimerOne.h>                                 // Header file for TimerOne library

#define trigPin 12                                    // Pin 12 trigger output
#define echoPin 2                                     // Pin 2 Echo input
#define onBoardLED 13                                 // Pin 13 onboard LED
#define echo_int 0                                    // Interrupt id for echo pulse

#define TIMER_US 50                                   // 50 uS timer duration 
#define TICK_COUNTS 4000                              // 200 mS worth of timer ticks

volatile long echo_start = 0;                         // Records start of echo pulse 
volatile long echo_end = 0;                           // Records end of echo pulse
volatile long echo_duration = 0;                      // Duration - difference between end and start
volatile int trigger_time_count = 0;                  // Count down counter to trigger pulse time
volatile long range_flasher_counter = 0;              // Count down counter for flashing distance LED

#define MAX_DISTANCE 400
// sets speed of DC  motors
#define MAX_SPEED 190
// Delay value for forward motor movement
#define MOTORDELAY 3
// Delay value for  backward motor movement
#define MOTORBACKWARDDELAY 3
// Delay for motor to 45 degree turns
#define TURNDELAY 75

int distance = 0;

// Motor A connections
int enA = 9;
int in1 = 6;
int in2 = 7;

// Motor B connections
int enB = 10;
int in3 = 4;
int in4 = 5;

int rightInfraredSensor = 13;  // This is our input pin
int leftInfraredSensor = 12;  // This is our input pin
int leftObsFound = 0;
int rightObsFound = 0;

void setup() {
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
  pinMode(trigPin, OUTPUT);                           // Trigger pin set to output
  pinMode(echoPin, INPUT);                            // Echo pin set to input
  pinMode(onBoardLED, OUTPUT);                        // Onboard LED pin set to output
  
  Timer1.initialize(TIMER_US);                        // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );                 // Attach interrupt to the timer service routine 
  attachInterrupt(echo_int, echo_interrupt, CHANGE);  // Attach interrupt to the sensor echo input
  Serial.begin(9600);
  
}

void loop() {
    int distance = echo_duration / 58;
    Serial.println(distance);

    if (distance >= 50) {
      digitalWrite( onBoardLED, HIGH);
      // Move forward
      moveFoward();
    } else {
      digitalWrite( onBoardLED, LOW);
    }
}

int checkRightInfraredSensor() {
    int isObstacleFound = 0;
    if (digitalRead(rightInfraredSensor) == LOW) {
      Serial.println("RIGHT SIDE OBSTACLE");
      isObstacleFound = 1;
    }

    return isObstacleFound;
}

int checkLeftInfraredSensor() {
    int isObstacleFound = 0;
    if (digitalRead(leftInfraredSensor) == LOW) {
      Serial.println("LEFT SIDE OBSTACLE");
      isObstacleFound = 1;
    }

    return isObstacleFound;
}

void takeRightTurn() {
  Serial.println("Take RIGHT turn");
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  delay(TURNDELAY);
}

void takeLeftTurn() {
  Serial.println("Take LEFT turn");
  // Turn off motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);  

  delay(TURNDELAY);
}

void stopMotors() {
  Serial.println("Motor STOPPED");
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void moveBackward() {
  Serial.println("Move BACK the car");
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
}

// This function lets you control speed of the motors
void moveFoward() {
  Serial.println("Move FORWARD the car");
  // Turn on motors
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
  // Accelerate from zero to maximum speed
  for (int i = 0; i < MAX_SPEED; i++) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(3);
  }
  
//  // Decelerate from maximum speed to zero
//  for (int i = MAX_SPEED; i >= 0; --i) {
//    analogWrite(enA, i);
//    analogWrite(enB, i);
//    delay(3);
//  }
}

// --------------------------
// timerIsr() 50uS second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{
    trigger_pulse();                                 // Schedule the trigger pulses
//    distance_flasher();                              // Flash the onboard LED distance indicator
}

// --------------------------
// trigger_pulse() called every 50 uS to schedule trigger pulses.
// Generates a pulse one timer tick long.
// Minimum trigger pulse width for the HC-SR04 is 10 us. This system
// delivers a 50 uS pulse.
// --------------------------
void trigger_pulse()
{
      static volatile int state = 0;                 // State machine variable

      if (!(--trigger_time_count))                   // Count to 200mS
      {                                              // Time out - Initiate trigger pulse
         trigger_time_count = TICK_COUNTS;           // Reload
         state = 1;                                  // Changing to state 1 initiates a pulse
      }
    
      switch(state)                                  // State machine handles delivery of trigger pulse
      {
        case 0:                                      // Normal state does nothing
            break;
        
        case 1:                                      // Initiate pulse
           digitalWrite(trigPin, HIGH);              // Set the trigger output high
           state = 2;                                // and set state to 2
           break;
        
        case 2:                                      // Complete the pulse
        default:      
           digitalWrite(trigPin, LOW);               // Set the trigger output low
           state = 0;                                // and return state to normal 0
           break;
     }
}

// --------------------------
// echo_interrupt() External interrupt from HC-SR04 echo signal. 
// Called every time the echo signal changes state.
//
// Note: this routine does not handle the case where the timer
//       counter overflows which will result in the occassional error.
// --------------------------
void echo_interrupt()
{
  switch (digitalRead(echoPin))                     // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      echo_end = 0;                                 // Clear the end time
      echo_start = micros();                        // Save the start time
      break;
      
    case LOW:                                       // Low so must be the end of hte echo pulse
      echo_end = micros();                          // Save the end time
      echo_duration = echo_end - echo_start;        // Calculate the pulse duration
      break;
  }
}
