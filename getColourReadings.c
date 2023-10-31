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
