//movement code

int 90deg = 400; //time taken to turn 90 degrees

void left_turn() {
  leftMotor.run(255);
  rightMotor.run(-255);
  delay(90deg);
}

void right_turn() {
  rightMotor.run(255);
  leftMotor.run(-255);
  delay(90deg);
}

void u_turn() {
  leftMotor.run(255);
  rightMotor.run(-255);
  delay(2 * 90deg);
}

void consec_right() {
  right_turn();
  delay(1000); //adjust time needed to go straight
  right_turn();
}

void consec_left() {
  left_turn();
  delay(1000);
  left_turn();
}
