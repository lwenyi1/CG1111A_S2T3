/**
 * A-maze-ing Race Mbot Arduino Code
 * 
 * @file B03_S2_T3_maincode.c
 * @author B03_S2_T3
 */

#include "MeMCore.h"

//define pins and components
#define LDR A2
#define IR_SENSOR A3
#define S1 A0
#define S2 A1
#define ULTRASONIC 12
MeBuzzer buzzer;
MeLineFollower lineFinder(PORT_2);
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);

//define coloured paper ID list
#define RED 0
#define GREEN 1
#define BLUE 2
#define ORANGE 3
#define PURPLE 4
#define WHITE 5

//define timings, cutoff values and speed
#define TIMEOUT 5000 
#define SPEED_OF_SOUND 340 
#define SAFEDISTANCE 9
#define COLOURSENSORCOOLDOWN 500
#define NINETYDEG 360
#define IRCUTOFF 300
#define CONSEC_TURN_WAIT_TIME 850
#define NUDGETIME 2
const uint8_t motorSpeed = 255;
const uint8_t turningSpeed = 255;

//calibrated colour values
float coloursArray[8][3] = 
{{289.90, 494.70, 300.70}, {181.70, 571.60, 316.70}, {210.80, 628.70, 485.80}, //red, green, blue
{296.80, 557.50, 323.20}, {216.00, 562.40, 414.80}, {328.40, 694.80, 528.10}, //orange, purple, white
{181.70, 494.70, 300.70}, {328.40, 694.80, 528.10}}; //min, max values for normalisation

/**
 * Plays a celebratory tune using the Mbot buzzer. Runs through two arrays, one
 * containing frequencies for the notes and one containing timings for the
 * rhythm
 */
void celebrate() 
{
  int notes[] = {784, 0, 784, 0, 880, 0, 1046, 0, 988, 0, 1175, 1319, 0, 1046};
  int rhythm[] = {18, 2, 18, 2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 60};
  for (int i = 0; i < 14; i++) 
  {
    if(notes[i] == 0) {
      buzzer.noTone();
      delay(rhythm[i] * 12);
    }
    else buzzer.tone(notes[i], rhythm[i] * 12);
  }
  buzzer.noTone();
}

/**
 * Writes the pins connected to the 2-4 decoder to HIGH or LOW depending on
 * the input.
 *
 * @param[in] mode An integer value corresponding to the desired decoder
 *                 output. 0 turns on the IR, 1 turns on the Red LED, 2 turns
 *                 on the Green LED, 3 turns on the Blue LED.
 * @param[out] S1 Pin for decoder's S1 input, set to HIGH or LOW.
 * @param[out] S2 Pin for decoder's S2 input, set to HIGH or LOW.
 */
void decoder(int mode) {
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

/**
 * Sends out and takes in a pulse from the ultrasonic sensor and measures the 
 * timing between, then converts it to distance measured. The distance measured
 * is used to check if the robot is too close or not, represented as a bool type.
 * Return value determines whether the robot is nudged or not.
 *
 * @param[in] TIMEOUT Definition for pulseIn time limit.
 * @param[in,out] ULTRASONIC Ultrasonic Sensor's pin, written to HIGH or LOW 
 *                           in OUTPUT mode. Reads in pulse when in INPUT mode.
 * @return Returns TRUE if distance measured is less than min. distance (too close)
 *         and FALSE if more than (not too close).
 */
bool read_ultrasonic()
{
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  long duration = pulseIn(ULTRASONIC, HIGH, TIMEOUT);
  double distance = duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100;
  return distance < SAFEDISTANCE && distance > 0;
}

/**
 * Reads in the voltage value from the IR_SENSOR pin and determines if
 * the robot is too close to the wall based on the measured value in
 * comparison to the cutoff value. Accounts for baseline IR light from
 * surroundings. Return value determines if the robot is nudged or not.
 *
 * @param[in] IR_SENSOR Pin taking in raw values from IR sensor's V_out.
 * @param[in] IRCUTOFF Raw reading corresponding to minimum distance from wall.
 * @return Returns TRUE if measured value is more than cut off (too close) and
 *         false if it is less (not too close).
 */
bool read_IR_sensor()
{
  decoder(3);
  long baseline = analogRead(IR_SENSOR);
  decoder(0);
  delay(10);
  long feedback = analogRead(IR_SENSOR);
  decoder(3);
  if (baseline - feedback > IRCUTOFF) return true;
  else return false;
}

/**
 * Makes a single reading of RGB values from the LDR, and iterates over all colours
 * in coloursArray, computing the difference between each RGB value of each colour
 * and the RGB values of the reading, normalising the difference with coloursArray[6]
 * and coloursArray[7], calculating the sum of squared error between the reading
 * and each colour, and returning the integer code of the colour that yields the
 * minimum sum of squared errors over the RGB values.
 *
 * @param[in] LDR Pin recieving raw values from colour detection circuit.
 * @param[in] coloursArray array of RGB values corresponding to the 6 colours of 
 *            paper, and the minimum and maximum RGB values.
 * @return Returns an integer value corresponding to the paper's colour, 
 *         according to coloured paper ID list.
 */
int detectColour()
{// Shine each colour, read LDR after some delay
  float readColour[3];
  for(int i = 0; i < 3; i++)
  {
    decoder(i+1);
    delay(COLOURSENSORCOOLDOWN);
    readColour[i] = analogRead(LDR);
  }
  float smallestError = 1470000;
  int colour = 5;
  for(int i = 0; i < 6; i++)
  {
    float sumSquareError = 0;
    for(int j = 0; j < 3; j++)
    {
      float normalisedError = (readColour[j] - coloursArray[i][j]) / (coloursArray[7][j] - coloursArray[6][j]);
      sumSquareError += normalisedError * normalisedError;
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
  leftMotor.stop();
  rightMotor.stop();
}

void moveForward()
{
  leftMotor.run(-motorSpeed);
  rightMotor.run(motorSpeed);
}

void turn_left()
{
  leftMotor.run(turningSpeed);
  rightMotor.run(turningSpeed);
  delay(NINETYDEG);
}

void turn_right()
{
  leftMotor.run(-turningSpeed);
  rightMotor.run(-turningSpeed);
  delay(NINETYDEG);
}

void turn_around()
{
  leftMotor.run(-turningSpeed);
  rightMotor.run(-turningSpeed);
  delay(NINETYDEG * 2);
}

void nudge_left()
{
  leftMotor.run(0);
  rightMotor.run(motorSpeed);
  delay(NUDGETIME);
}

void nudge_right()
{
  leftMotor.run(-motorSpeed);
  rightMotor.run(0);
  delay(NUDGETIME);
}


void setup() 
{
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  decoder(2);
  delay(2000);
  decoder(3);
}

void loop()
{
  if (lineFinder.readSensors() == S1_IN_S2_IN) 
  {
    stopMotor();
    int colour = detectColour();
    //steering instructions for robot based on colour
    if (colour == RED) turn_left();
    else if (colour == GREEN) turn_right();
    else if (colour == ORANGE) turn_around();
    else if (colour == PURPLE)
    {
      turn_left();
      moveForward();
      delay(CONSEC_TURN_WAIT_TIME);
      turn_left();
    }
    else if(colour == BLUE)
    {
      turn_right();
      moveForward();
      delay(CONSEC_TURN_WAIT_TIME);
      turn_right();
    }
    else
    {
      stopMotor();
      celebrate();
      delay(5000);
    }
  }
  else if (read_ultrasonic()) nudge_right();
  else if (read_IR_sensor()) nudge_left();
  else moveForward();
}
