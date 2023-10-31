#include <MeMCore.h>
#define LIGHTSENSOR A2
#define ULTRASONIC 12

#define TIMEOUT 1000 // Max microseconds to wait; choose according to max distance of wall
#define SPEED_OF_SOUND 340 // Update according to your own experiment

int IRBaseLine;
MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
MeLineFollower lineFinder(PORT_2); // assigning lineFinder to RJ25 port 2
uint8_t motorSpeed = 255;

int closeToLeft = 0;
int closeToRight = 0;

void celebrate() 
{// Code for playing celebratory tune
}
void stopMotor()
{
  leftMotor.stop(); // Stop left motor
  rightMotor.stop(); // Stop right motor
}
void moveForward() 
{
  // Code for moving forward for some short interval
}
void turnRight() 
{
  // Code for turning right 90 deg
}
void turnLeft() 
{// Code for turning left 90 deg
}
void uTurn() 
{
  // Code for u-turn
}
void doubleLeftTurn() 
{// Code for double left turn
}
void doubleRightTurn() 
{
  // Code for double right turn
}
void nudgeLeft() 
{
  // Code for nudging slightly to the left for some short interval
  stopMotor();
  rightMotor.run(motorSpeed);
  delay(50);
}
void nudgeRight() 
{// Code for nudging slightly to the right for some short interval
  stopMotor();
  leftMotor.run(-motorSpeed);
  delay(50);
}
void shineIR() {// Code for turning on the IR emitter only
}
void shineRed() {// Code for turning on the red LED only
}
void shineGreen() {// Code for turning on the green LED only
}
void shineBlue() {// Code for turning on the blue LED only
}
int detectColour()
{
// Shine Red, read LDR after some delay
// Shine Green, read LDR after some delay
// Shine Blue, read LDR after some delay
// Run algorithm for colour decoding
}
void setup()
{
// Configure pinMode for A0, A1, A2, A3
  Serial.begin(9600);
  IRBaseLine = analogRead(LIGHTSENSOR);
}
void loop()
{
  

// Read ultrasonic sensing distance (choose an appropriate timeout)
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  long duration = pulseIn(ULTRASONIC, HIGH, TIMEOUT);
  int dist_cm = duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100;
  Serial.println(closeToLeft);
  if (dist_cm < 6 && dist_cm != 0) 
  {
    closeToLeft++;
  } 

// Read IR sensing distance (turn off IR, read IR detector, turn on IR, read IR detector, estimate distance)
  if(analogRead(LIGHTSENSOR) - IRBaseLine > 100)
  {
    closeToRight++;
  }

// if within black line, stop motor, detect colour, and take corresponding action

  
// else if too near to left wall, nudge right
  if(closeToLeft >= 8)
  {
    closeToLeft = 0;
    Serial.println("nudge right");
    nudgeRight();
  }
// else if too near to right wall, nudge left
  else if(closeToRight >= 8)
  {
    closeToRight = 0;
    nudgeLeft();
  }


  else if(lineFinder.readSensors() == S1_IN_S2_IN)
  {
    stopMotor();
    colour = detectColour() ;
    if ( colour == green )
    {
      turnRight() ;
    }
    else if ( colour == purple )
    {
      doubleLeftTurn() ;
    }
    else if ( colour == orange )
    {
      uTurn() ;
    }
    else if ( colour == blue )
    {
      doubleRightTurn() ;
    }
    else if ( colour == red )
    {
      leftTurn() ;
    }
    else 
    {
      celebrate() ;
      delay( 10000 ) ;
    }
  }
// else move forward
  else
  {
    //go forward
    leftMotor.run(-motorSpeed);
    rightMotor.run(motorSpeed);
  }
  
}

