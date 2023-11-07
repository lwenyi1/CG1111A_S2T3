#include "MeMCore.h"

#define LDR A2
#define IR_SENSOR A3
#define S1 A0
#define S2 A1

#define TIMEOUT 5000 
#define SPEED_OF_SOUND 340 
#define ULTRASONIC 12
#define SAFEDISTANCE 9
#define COLOURSENSORCOOLDOWN 500
#define NINETYDEG 640
#define IRCUTOFF 300

MeBuzzer buzzer;
MeLineFollower lineFinder(PORT_2); // assigning lineFinder to RJ25 port 2

MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
uint8_t motorSpeed = 255;// Setting motor speed to an integer between 1 and 255
uint8_t turningSpeed = 170;

float coloursArray[6][3] = {{241.20, 449.00, 224.60}, {126.10, 540.00, 229.50}, {150.90, 603.20, 417.70}, {255.80, 533.60, 255.60}, {162.10, 528.40, 347.00}, {272.20, 685.10, 472.30}};
char colourStr[6][7] = {"Red", "Green", "Blue", "Orange", "Purple", "White"};
int notes[] = {784, 0, 784, 0, 880, 0, 1046, 0, 988, 1175, 1319, 0, 1046};
int rhythm[] = {19, 1, 18, 2, 18, 2, 20, 2, 18, 10, 18, 5, 70};

void celebrate() {
  // Each of the following "function calls" plays a single tone.
  // The numbers in the bracket specify the frequency and the duration (ms)
  for (int i = 0; i < 13; i++) {
    if(notes[i] == 0)
    {
      buzzer.noTone();
      delay(rhythm[i] * 12);
    }
    else buzzer.tone(notes[i], rhythm[i] * 12);
  }
  buzzer.noTone();
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

bool read_IR_sensor() {
  long baseline = analogRead(IR_SENSOR);
  decoder(0);
  delay(10);
  long feedback = analogRead(IR_SENSOR);
  if (baseline - feedback > IRCUTOFF) {
    decoder(3);
    return true;
  }
  decoder(3);
  return false;
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
  float smallestError = 1470000;
  int colour;
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

void turn_left(){
  leftMotor.run(turningSpeed);
  rightMotor.run(turningSpeed);
  delay(NINETYDEG);
}

void turn_right() {
  leftMotor.run(-turningSpeed);
  rightMotor.run(-turningSpeed);
  delay(NINETYDEG);
}

void turn_around() {
  leftMotor.run(-turningSpeed);
  rightMotor.run(-turningSpeed);
  delay(NINETYDEG * 2);
}

void nudge_left() {
  leftMotor.run(0);
  rightMotor.run(motorSpeed);
  delay(2);
}

void nudge_right() {
  leftMotor.run(-motorSpeed);
  rightMotor.run(0);
  delay(2);
}

void setup() 
{
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  decoder(2);
  delay(3000);
  decoder(3);
}

void loop() {
  double distance = read_ultrasonic();//read the distance from ultranosic sensor
  bool IR_bool = read_IR_sensor();
  int sensorState = lineFinder.readSensors(); // read the line sensor's state
  if (sensorState == S1_IN_S2_IN) 
  {
    stopMotor();
    int red = 0, green = 1, blue = 2, orange = 3, purple = 4, white = 5, colour = detectColour();
    if (colour == red) turn_left();
    else if (colour == green) turn_right();
    else if (colour == orange) turn_around();
    else if (colour == purple)
    {
      turn_left();
      moveForward();
      delay(850);
      turn_left();
    }
    else if(colour == blue)
    {
      turn_right();
      moveForward();
      delay(850);
      turn_right();
    }
    else
    {
      stopMotor();
      celebrate();
      delay(5000);
    }
  }
  else if (distance < SAFEDISTANCE && distance > 0) nudge_right();
  else if (IR_bool) nudge_left();
  else moveForward();
}
