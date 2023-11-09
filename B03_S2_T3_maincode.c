/**
 * @file B03_S2_T3_maincode.c
 * @brief A-maze-ing Race Mbot Arduino Code
 * @details This Arduino code determines how our Mbot behaves while going through the maze.
 *          It contains functions for decoder output, Ultrasonic sensor readings, IR sensor
 *          readings, Colour detection, steering and the celebration. It also has the Arduino
 *          setup and loop functions.
 *
 * @author B03_S2_T3
 */

#include "MeMCore.h"

//define pins and components
#define LDR A2 ///< Pin connected to V_out at LDR in colour detector circuit
#define IR_SENSOR A3 ///< Pin connected to V_out in IR sensor circuit
#define S1 A0 ///< Pin connected to decoder S1 pin
#define S2 A1 ///< Pin connected to decoder S2 pin
#define ULTRASONIC 12 ///< Pin connected to Ultrasonic sensor
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
#define TIMEOUT 5000 ///< Time limit for Ultrasonic sensor
#define SPEED_OF_SOUND 340 ///< Speed of sound chosen for calcs is 340m/s
#define SAFEDISTANCE 9 ///< Limit before robot considered too close to wall, in Ultrasonic sensor
#define COLOURSENSORCOOLDOWN 50
#define NINETYDEG 360 ///< Delay needed to turn 90 degrees
#define IRCUTOFF 300 ///< Limit before robot considered too close to wall, in IR sensor
#define CONSEC_TURN_WAIT_TIME 850 ///< Delay between consecutive turns
#define NUDGETIME 2 ///< Delay determining amount robot is nudged
const uint8_t motorSpeed = 255; ///< Determines speed going forward, max value of 255
const uint8_t turningSpeed = 255; ///< Determines speed while turning, max value of 255

//calibrated colour values
float coloursArray[8][3] = 
{{289.90, 494.70, 300.70}, {181.70, 571.60, 316.70}, {210.80, 628.70, 485.80}, //red, green, blue
{296.80, 557.50, 323.20}, {216.00, 562.40, 414.80}, {328.40, 694.80, 528.10}, //orange, purple, white
{181.70, 494.70, 300.70}, {328.40, 694.80, 528.10}}; //min, max values for normalisation

/**
 * Writes the output pins connected to the 2-4 decoder to HIGH or LOW depending on
 * the input.
 *
 * @param[in] mode An integer value corresponding to the desired decoder
 *                 output. 0: turns on the IR. 1: turns on the Red LED. 2: turns
 *                 on the Green LED. 3: turns on the Blue LED.
 * @pre Caller must ensure that mode is a valid value from 0 to 3. Function will consider
 *      any input value outside the range as 3 and turn on the Blue LED.
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

/**
 * The following steering functions determine the robots movement by varying
 * inputs to the left and right motor functions provided by the Mbot library.
 */
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

/**
 * Plays a celebratory tune using the Mbot buzzer. Runs through two arrays, one
 * containing frequencies for the notes and one containing timings for the
 * rhythm
 */
void celebrate() 
{
  int notes[] = 
  {392, 523, 659, 784, 1046, 1319, 1568, 1319, 0,
  415, 523, 622, 831, 1046, 1245, 1661, 1245, 0, 
  466, 587, 698, 932, 1175, 1397, 1865, 0,
  1865, 0, 1865, 0, 1865, 0, 2093};

  int rhythm[] =
  {20, 20, 20, 20, 20, 20, 60, 40, 20,
  20, 20, 20, 20, 20, 20, 60, 40, 20,
  20, 20, 20, 20, 20, 20, 40, 20,
  10, 10, 10, 10, 10, 10, 120};

  for (int i = 0; i < 33; i++) 
  {
    if(notes[i] == 0)
    {
      buzzer.noTone();
      delay(rhythm[i] * 8);
    }
    else buzzer.tone(notes[i], rhythm[i] * 8);
  }
  buzzer.noTone();
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
    else if (colour == PURPLE) //consecutive left turns
    {
      turn_left();
      moveForward();
      delay(CONSEC_TURN_WAIT_TIME);
      turn_left();
    }
    else if(colour == BLUE) //consecutive right turns
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
