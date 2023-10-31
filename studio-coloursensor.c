//FOR LIGHTSENSOR

// Define time delay before taking another LDR reading
#define LDRWait 10 //in milliseconds

// Define time delay before changing to another LED colour
#define RGBWait 100

#define LDR 0   //LDR sensor pin at A0
#define LED 13  //Check Indicator to signal Calibration Completed

// Define colour sensor LED pins
int S1 = A2;
int S2 = A3;

//placeholders for colour detected
int red = 0;
int green = 0;
int blue = 0;

//floats to hold colour arrays
float colourArray[] = {0,0,0};
float whiteArray[] = {0,0,0};
float blackArray[] = {0,0,0};
float greyDiff[] = {0,0,0};

void setup(){
char colourStr[3][6] = {"Red", "Green", "Blue"};
int currentReading;
     //setup the outputs for the colour sensor
      for (int c = 0;c<=2;c++){
        pinMode(ledArray[c],OUTPUT);   
      }

     pinMode(LED,OUTPUT);  //Check Indicator -- OFF during Calibration

     //begin serial communication
     Serial.begin(9600);

     setBalance();  //calibration
     digitalWrite(LED, HIGH); //Check Indicator -- ON after Calibration
   
     for(int c = 0;c<=2;c++){    //one colour at a time
        Serial.println(colourStr[c]);

        digitalWrite(ledArray[c],HIGH); //turn ON the LED, red, green or blue, one colour at a time
        for(int i = 0; i< 50; i++){  //taking one sample at a time, total of 50 samples
              currentReading = (analogRead(LDR) - blackArray[c])/(greyDiff[c])*255; 
              printColour( currentReading);
         } 
         digitalWrite(ledArray[c],LOW); //turn OFF the LED, red, green or blue depending which iteration
	 delay(RGBWait);
  } 
    Serial.println("Experiment Completed.");
}

void loop(){
 //everything is done once in setup()
}

void setBalance(){
//set white balance
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);     //delay for five seconds for getting sample ready
  digitalWrite(LED,LOW); //Check Indicator OFF during Calibration
//scan the white sample.
//go through one colour at a time, set the maximum reading for each colour -- red, green and blue to the white array
  for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);
     delay(RGBWait);
     whiteArray[i] = getAvgReading(5);         //scan 5 times to take the average, 
     digitalWrite(ledArray[i],LOW);
     delay(RGBWait);
  }
//done scanning white, time for the black sample.

//set black balance
  Serial.println("Put Black Sample For Calibration ...");
    delay(5000);     //delay for five seconds for getting sample ready 
//go through one colour at a time, set the minimum reading for red, green and blue to the black array
  for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);
     delay(RGBWait);
     blackArray[i] = getAvgReading(5);
     digitalWrite(ledArray[i],LOW);
     delay(RGBWait);
//the differnce between the maximum and the minimum gives the range
     greyDiff[i] = whiteArray[i] - blackArray[i];
  }

  //delay another 5 seconds to allow us to prepare the colour objects
  Serial.println("Put Colour Sample For Data Collection  ...");
  delay(5000); //get ready the colour paper
}

int getAvgReading(int times){      
//find the average reading for the requested number of times of scanning LDR
  int reading;
  int total =0;
//take the reading as many times as requested and add them up
  for(int i = 0;i < times;i++){
     reading = analogRead(LDR);
     total = reading + total;
     delay(LDRWait);
  }
//calculate the average and return it
  return total/times;
}
