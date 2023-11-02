//Celebration music (Never Gonna Give You Up intro)
#include <MeMCore.h>
MeBuzzer buzzer; // create the buzzer object

int notes[] =
{554, 622, 622, 698, 831, 740, 698, 622, 554, 622, 0, 415, 415};

int rhythm[] =
{6, 10, 6, 6, 1, 1, 1, 1, 6, 10, 4, 2, 10};

void celebrate() {
  // Each of the following "function calls" plays a single tone.
  // The numbers in the bracket specify the frequency and the duration (ms)
  for (int i = 0; i < 14; i++) {
    buzzer.tone(notes[i], rhythm[i] * 100);
  }
  buzzer.noTone();
}
void setup() {
  // Any setup code here runs only once:
}
void loop() {
  // The main code here will run repeatedly (i.e., looping):
  celebrate(); // play the tune specified in the function celebrate()
  delay(1000); // pauses for 1000 ms before repeating the loop
}
