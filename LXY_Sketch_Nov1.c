#include "MeMCore.h"
#define TIMEOUT 5000 
#define SPEED_OF_SOUND 340 
#define ULTRASONIC 12
#define safe_distance 9

#define IR_SENSOR A3
#define INPUT_1A A0
#define INPUT_1B A1

MeLineFollower lineFinder(PORT_2); // assigning lineFinder to RJ25 port 2

MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
uint8_t motorSpeed = 255;// Setting motor speed to an integer between 1 and 255

void setup() {
Serial.begin(9600); // to initialize the serial monitor
pinMode(INPUT_1A, OUTPUT);
pinMode(INPUT_1B, OUTPUT);
digitalWrite(INPUT_1A, HIGH);
digitalWrite(INPUT_1B, HIGH);
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
  digitalWrite(INPUT_1A, LOW);
  digitalWrite(INPUT_1B, LOW);
  delay(10);
  long feedback = analogRead(IR_SENSOR);
  if (baseline - feedback > 250) {
    digitalWrite(INPUT_1A, LOW);
    digitalWrite(INPUT_1B, HIGH);
    Serial.print("baseline");
    Serial.print(" = ");
    Serial.println(baseline);
    Serial.println(feedback);
    return 1;
  }
  digitalWrite(INPUT_1A, LOW);
  digitalWrite(INPUT_1B, HIGH);
  Serial.println(analogRead(feedback));
  return 0;
}

void loop() {
  double distance = read_ultrasonic();//read the distance from ultranosic sensor
  int IR_bool = read_IR_sensor();
  int sensorState = lineFinder.readSensors(); // read the line sensor's state
  if (sensorState == S1_IN_S2_IN) { // situation 1
    leftMotor.run(0); // Left wheel halt
    rightMotor.run(0); // Right wheel halt
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
}

