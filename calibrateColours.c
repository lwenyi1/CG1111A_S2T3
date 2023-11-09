#include <MeMCore.h>
#define LIGHTSENSOR A3
#define ULTRASONIC 12
#define LDR A2
#define S1 A0
#define S2 A1

#define TIMEOUT 1000 // Max microseconds to wait; choose according to max distance of wall
#define SPEED_OF_SOUND 340 // Update according to your own experiment
#define COLOURSENSORCOOLDOWN 50 // timeout in ms for coloursensor

float coloursArray[8][3] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {10000,10000,10000}, {0,0,0}};
char colourStr[6][7] = {"Red", "Green", "Blue", "Orange", "Purple", "White"};

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
  //DEBUG
  Serial.print("(");
  for(int k = 0; k < 3; k++)
    {
      Serial.print(colourStr[k]);
      Serial.print(":");
      Serial.print(readColour[k]);
      Serial.print(" ");
    }
  Serial.print(")");
  Serial.println(" ");
  //DEBUG
// Run algorithm for colour decoding
  float smallestError = 1470000, colour = 2;
  for(int i = 0; i < 6; i++)
  {
    float sumSquareError = 0;
    for(int j = 0; j < 3; j++)
    {
      float normalisedError = (readColour[j] - coloursArray[i][j]) / (coloursArray[7][j] - coloursArray[6][j]);
      sumSquareError += normalisedError * normalisedError;
    }
    //DEBUG
    Serial.print(colourStr[i]);
    Serial.print(" error: ");
    Serial.println(sumSquareError);
    //DEBUG
    if (sumSquareError < smallestError)
    {
      colour = i;
      smallestError = sumSquareError;
    }
  }
  return colour;
}

void getColourReadings(int scansPerColour){
  for(int i = 0; i < 6; i++)
  {
    decoder(0);
    Serial.println("Colour calibrating:");
    Serial.println(colourStr[i]);
    delay(5000);     //delay for five seconds for getting sample ready
    Serial.println("Scanning...");
    for(int j = 0; j < scansPerColour; j++)
    {
      for(int k = 0; k < 3; k++)
      {
        decoder(k + 1);
        delay(COLOURSENSORCOOLDOWN);
        coloursArray[i][k] += analogRead(LDR);
      }
    }
    for(int k = 0; k < 3; k++)
    {
      coloursArray[i][k] /= scansPerColour;
      if(coloursArray[i][k] < coloursArray[6][k]) coloursArray[6][k] = coloursArray[i][k];
      if(coloursArray[i][k] > coloursArray[7][k]) coloursArray[7][k] = coloursArray[i][k];
    }
    Serial.println("Colour scanned.");
    delay(2000);
  }

  Serial.println("All colours scanned.");
  Serial.print("{");
  for(int i = 0; i < 8; i++)
  {
    Serial.print("{");
    for(int k = 0; k < 3; k++)
    {
      Serial.print(coloursArray[i][k]);
      if(k != 2) Serial.print(", ");
    }
    if(i != 7) Serial.print("}, ");
   
  }
  Serial.print("}};");
  Serial.println(" ");
  delay(5000);
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

void setup(){
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  Serial.begin(9600);
  getColourReadings(10);
}

void loop(){
 delay(3000);
 int ans = detectColour();
 Serial.println("colour detected:");
 Serial.println(colourStr[ans]);
 Serial.println("---------");
}

