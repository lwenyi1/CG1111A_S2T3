#include <MeMCore.h>
#define LIGHTSENSOR A3
#define ULTRASONIC 12
#define LDR A2
#define S1 A0
#define S2 A1

#define TIMEOUT 1000 // Max microseconds to wait; choose according to max distance of wall
#define SPEED_OF_SOUND 340 // Update according to your own experiment
#define COLOURSENSORCOOLDOWN 500
#define IRSENSORCOOLDOWN 10
#define NINETYDEG 230

MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
MeLineFollower lineFinder(PORT_2); // assigning lineFinder to RJ25 port 2
uint8_t motorSpeed = 255;

int closeToLeft = 0;
int closeToRight = 0;
float coloursArray[6][3] = {{231.00,219.60,257.80}, {64.00,265.60,223.80}, {88.80,352.80,442.00}, {257.40,328.80,317.20}, {102.00,244.40,354.40}, {293.40,495.80,523.00}};
/*
Red: (Red:231.00 Green:219.60 Blue:257.80 ) 
Green: (Red:64.00 Green:265.60 Blue:223.80 ) 
Blue: (Red:88.80 Green:352.80 Blue:442.00 ) 
Orange: (Red:257.40 Green:328.80 Blue:317.20 ) 
Purple: (Red:102.00 Green:244.40 Blue:354.40 ) 
White: (Red:293.40 Green:495.80 Blue:523.00 )
*/


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
  leftMotor.run(-motorSpeed);
  rightMotor.run(motorSpeed);
}
void turnLeft() 
{
  rightMotor.run(255);
  leftMotor.run(255);
  delay(NINETYDEG);
  moveForward();
}
void turnRight() 
{
  leftMotor.run(-255);
  rightMotor.run(-255);
  delay(NINETYDEG);
  moveForward();
}
void uTurn() 
{
  leftMotor.run(255);
  rightMotor.run(255);
  delay(2 * NINETYDEG);
  moveForward();
}
void doubleLeftTurn() 
{
  turnLeft();
  moveForward();
  delay(1000);
  turnLeft();
  moveForward();
}
void doubleRightTurn() 
{
  turnRight();
  moveForward();
  delay(1000); //adjust time needed to go straight
  turnRight();
  moveForward();
}
void nudgeLeft() 
{
  // Code for nudging slightly to the left for some short interval
  rightMotor.run(motorSpeed);
  delay(50);
}
void nudgeRight() 
{// Code for nudging slightly to the right for some short interval
  leftMotor.run(-motorSpeed);
  delay(50);
}

void decoder(int mode)
{ //0 for IR, 1 for Red, 2 for Green, 3 for Blue
  if (mode == 0)
  {
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
  }
  else if (mode == 1)
  {
    digitalWrite(S1, HIGH);
    digitalWrite(S2, HIGH);
  }
  else if (mode == 2)
  {
    digitalWrite(S1, HIGH);
    digitalWrite(S2, LOW);
  }
  else
  {
    digitalWrite(S1, LOW);
    digitalWrite(S2, HIGH);
  }
}

int detectColour()
{
// Shine each colour, read LDR after some delay
  float readColour[3];
  for(int i = 0; i < 3; i++)
  {
    decoder(i+1);
    delay(COLOURSENSORCOOLDOWN);
    readColour[i] = analogRead(LDR);
  }
// Run algorithm for colour decoding
  float smallestError = 1470000, colour = 2;
  for(int i = 0; i < 6; i++)
  {
    float sumSquareError = 0;
    for(int j = 0; j < 3; j++)
    {
      float error = readColour[j] - coloursArray[i][j];
      sumSquareError += error * error;
    }
    if (sumSquareError < smallestError)
    {
      colour = i;
      smallestError = sumSquareError;
    }
  }
  return colour;
}

void setup()
{
// Configure pinMode for A0, A1, A2, A3
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  
  Serial.begin(9600);
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
  if (dist_cm < 8 && dist_cm != 0) 
  {
    closeToLeft++;
  } 

// Read IR sensing distance (turn off IR, read IR detector, turn on IR, read IR detector, estimate distance)
  decoder(1);
  int IRBaseline = analogRead(LIGHTSENSOR);
  decoder(0);
  delay(IRSENSORCOOLDOWN);
  if(IRBaseline - analogRead(LIGHTSENSOR)> 230)
  {
    closeToRight++;
  }

// if within black line, stop motor, detect colour, and take corresponding action
  if(lineFinder.readSensors() == S1_IN_S2_IN)
  {
    stopMotor();
    int colour = detectColour();
    int red = 0, green = 1, blue = 2, orange = 3, purple = 4;
    if (colour == red)
    {
      turnLeft();
    }
    else if (colour == green)
    {
      turnRight() ;
    }
    else if (colour == blue)
    {
      doubleRightTurn() ;
    }
    else if (colour == orange)
    {
      uTurn() ;
    }
    else if (colour == purple)
    {
      doubleLeftTurn() ;
    }
    else 
    {
      celebrate();
    }
  }
  
// else if too near to left wall, nudge right
  else if(closeToLeft >= 5)
  {
    closeToLeft = 0;
    nudgeRight();
  }

// else if too near to right wall, nudge left
  else if(closeToRight >= 5)
  {
    closeToRight = 0;
    nudgeLeft();
  }

// else move forward
  else
  {
    moveForward();
  }
  
}
