#include "MeMCore.h"
#define TIMEOUT 5000 
#define SPEED_OF_SOUND 340 
#define ULTRASONIC 12
#define safe_distance 8

#define LDR A2
#define IR_SENSOR A3

#define S1 A0
#define S2 A1

#define COLOURSENSORCOOLDOWN 500

MeBuzzer buzzer;
MeLineFollower lineFinder(PORT_2); // assigning lineFinder to RJ25 port 2

MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
uint8_t motorSpeed = 255;// Setting motor speed to an integer between 1 and 255
uint8_t turningSpeed = 170;

float coloursArray[6][3] = {{369.8,523.8,347.6}, {263.1,588.4,339.9}, {269.6,637.2,476.7}, {380.5,588.5,374.9}, {279.0,574,422.1}, {410,715.4,546.5}};
char colourStr[6][7] = {"Red", "Green", "Blue", "Orange", "Purple", "White"};

void setup() {
pinMode(S1, OUTPUT);
pinMode(S2, OUTPUT);
decoder(2);
delay(3000);
decoder(3);
}

void decoder(int mode)
{ //0 for IR, 1 for Red, 2 for Green, 3 for Blue
  if (mode == 0) {
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
  }
  else if (mode == 1) {
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

double read_ultrasonic() {
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  long duration = pulseIn(ULTRASONIC, HIGH, TIMEOUT);
  double distance = duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100;
  return distance;
}

int read_IR_sensor() {
  long baseline = analogRead(IR_SENSOR);
  decoder(0);
  delay(10);
  long feedback = analogRead(IR_SENSOR);
  if (baseline - feedback > 250) {
    decoder(3);
    return 1;
  }
  decoder(3);
  return 0;
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
  float smallestError = 1470000, colour = 2;
  for(int i = 0; i < 6; i++)
  {
    float sumSquareError = 0;
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

void turn_left(){
    leftMotor.run(turningSpeed);
    rightMotor.run(turningSpeed);
    delay(640);
    leftMotor.run(0);
    rightMotor.run(0);
    delay(500);
}

void turn_right() {
    leftMotor.run(-turningSpeed);
    rightMotor.run(-turningSpeed);
    delay(640);
    leftMotor.run(0);
    rightMotor.run(0);
    delay(500);
}

void turn_around() {
    leftMotor.run(turningSpeed);
    rightMotor.run(turningSpeed);
    delay(1280);
    leftMotor.run(0);
    rightMotor.run(0);
    delay(500);
}

int colour = 0;

void loop() {
  if (colour != 5) {
  double distance = read_ultrasonic();//read the distance from ultranosic sensor
  int IR_bool = read_IR_sensor();
  int sensorState = lineFinder.readSensors(); // read the line sensor's state
  if (sensorState == S1_IN_S2_IN) {
    leftMotor.run(0); // Left wheel halt
    rightMotor.run(0); // Right wheel halt
    colour = detectColour();
    if(colour == 0) {
      turn_left();
    }
    if(colour == 1) {
      turn_right();
    }
    if(colour == 3) {
      turn_around();
    }
    if(colour == 4) {
      turn_left();
      leftMotor.run(-motorSpeed);
      rightMotor.run(motorSpeed);
      delay(900);
      turn_left();
    }
    if(colour == 2) {
      turn_right();
      leftMotor.run(-motorSpeed);
      rightMotor.run(motorSpeed);
      delay(900);
      turn_right();
    }
  } else if (distance < safe_distance && distance > 0) {
    leftMotor.run(-motorSpeed);
    rightMotor.run(0);
    delay(2); // Adjust this delay as needed
  } else if (IR_bool == 1) {
    leftMotor.run(0);
    rightMotor.run(motorSpeed);
  } else {
    leftMotor.run(-motorSpeed);
    rightMotor.run(motorSpeed);
  }
  } else {
      leftMotor.run(0);
      rightMotor.run(0);
      buzzer.tone(392, 200);
    buzzer.tone(523, 200);
    buzzer.tone(659, 200);
    buzzer.tone(784, 200);
    buzzer.tone(659, 150);
    buzzer.tone(784, 400);
    buzzer.noTone();
    delay(5000);
  }
}
