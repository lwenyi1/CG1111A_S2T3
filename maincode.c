#include <MeMCore.h>
#define LIGHTSENSOR A2
#define ULTRASONIC 12
#define LDR A3
#define S1 A0
#define S2 A1

#define TIMEOUT 1000 // Max microseconds to wait; choose according to max distance of wall
#define SPEED_OF_SOUND 340 // Update according to your own experiment
#define COLOURSENSORCOOLDOWN 50 // timeout in ms for coloursensor
#define NINETY 230 // time taken to turn 90 deg

MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
MeLineFollower lineFinder(PORT_2); // assigning lineFinder to RJ25 port 2
uint8_t motorSpeed = 255;

int closeToLeft = 0;
int closeToRight = 0;
float coloursArray[6][3] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};

void celebrate() 
{// Code for playing celebratory tune
}
void stopMotor()
{
  leftMotor.stop(); // Stop left motor
  rightMotor.stop(); // Stop right motor
}
void go_straight() 
{
  leftMotor.run(255);
  rightMotor.run(-255);
}

void left_turn() {
  leftMotor.run(255);
  rightMotor.run(255);
  delay(NINETY);
  go_straight() ;
}

void right_turn() {
  rightMotor.run(-255);
  leftMotor.run(-255);
  delay(NINETY+10);
  go_straight() ;
}

void u_turn() {
  leftMotor.run(-255);
  rightMotor.run(-255);
  delay(3.7 * NINETY);
  go_straight() ;
}

void consec_right() {
  right_turn();
  go_straight() ;
  delay(1000); //adjust time needed to go straight
  right_turn();
  go_straight() ;
}

void consec_left() {
  left_turn();
  go_straight();
  delay(1000);
  left_turn();
  go_straight() ;
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

void decoder(int mode)
{ //0 for IR, 1 for Red, 2 for Green, 3 for Blue
  if (mode == 0) {
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
  } else if (mode == 1) {
    digitalWrite(S1, HIGH);
    digitalWrite(S2, HIGH);
  } else if (mode == 2) {
    digitalWrite(S1, HIGH);
    digitalWrite(S2, LOW);
  } else {
    digitalWrite(S1, LOW);
    digitalWrite(S2, HIGH);
  }
}


int detectColour()
{
// Shine each colour, read LDR after some delay
  int readColour[3];
  for(int i = 0; i < 3; i++)
  {
    decoder(i+1);
    delay(COLOURSENSORCOOLDOWN);
    readcolour[i] = analogRead(LDR);
  }
// Run algorithm for colour decoding
  int smallestError = 2147483647, colour = 0;
  for(int i = 0; i < 6; i++)
  {
    int sumSquareError = 0;
    for(int j = 0; j < 3; j++)
    {
      sumSquareError += (readColour[j] - coloursArray[i][j]) * (readColour[j] - coloursArray[i][j]);
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
  Serial.println(closeToLeft);
  if (dist_cm < 6 && dist_cm != 0) 
  {
    closeToLeft++;
  } 

// Read IR sensing distance (turn off IR, read IR detector, turn on IR, read IR detector, estimate distance)
  decoder(1);
  int IRBaseline = analogread(LIGHTSENSOR);
  decoder(0);
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
    int colour = detectColour();
    int red = 0, green = 1, blue = 2, orange = 3, purple = 4;
    if (colour == red)
    {
      leftTurn() ;
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
