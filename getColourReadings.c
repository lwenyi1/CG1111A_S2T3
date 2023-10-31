void getColourReadings(int scansPerColour){
  for(int i = 0; i < 6; i++)
  {
    Serial.println("Colour calibrating:");
    Serial.println(colourStr[i]);
    delay(5000);     //delay for five seconds for getting sample ready
    Serial.println("Scanning...")
    for(int j = 0; j < scansPerColour; j++)
    {
      for(int k = 1; k <= 3; k++)
      {
        decoder(k);
        delay(50);
        coloursArray[i][k - 1] += analogRead(A3);
      }
    }
    for(int k = 0; k < 3; k++)
    {
      coloursArray[i][k] /= scansPerColour;
    }
    Serial.println("Colour scanned.");
    delay(2000);
  }

  void decoder(int mode) { //0 for IR, 1 for Red, 2 for Green, 3 for Blue
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

  Serial.println("All colours scanned.");
  for(int i = 0; i < 6; i++)
  {
    Serial.print(colourStr[i]);
    Serial.print(": (");
    for(int k = 0; k < 3; k++)
    {
      Serial.print(colourStr[k]);
      Serial.print(":");
      Serial.print(coloursArray[i][k]);
      Serial.print(" ");
    }
    Serial.print(")");
    Serial.println(" ");
  }
}
