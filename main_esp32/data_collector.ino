/*
   #################################
   ## Ethereum Transaction Issuer ##
   #################################

   This component collects data from the analog input pin.
   Currently, it measures brightness values from 0-1024.
   
   Author: David Michel
*/


/*
  Returns the analog pin's value as an integer.
  @return The integer value.
*/
int getAnalogInput()
{
  //value = analogRead(A0);
  int value = 1000;
  Serial.println("\nAnalog value is:");
  Serial.println(value);
  return value;
}

/*
  Returns the analog pin's value as a hex prefixed string.
  @return The sensor value as a string in hex.
*/
String getAnalogInputHex()
{
  //value = analogRead(A0);
  //value = 20;
  int value = getUltrasonicDistance();
  String output = "0x" + intToHex(value);
  Serial.print("\nDistance:");
  Serial.print(value);
  Serial.println("mm");
  return output;
}

//int getUltrasonicDistance()
//{ 
//  const int TrigPin = D0; //Send out ultrasound
//  const int EchoPin = D1; //Received reflected echoes
//  pinMode(TrigPin, OUTPUT); 
//  pinMode(EchoPin, INPUT); 
//  float mm; //distance
//  int mm_int;
//  bool valid_value = false;   //validty of the distance value
//  while(valid_value == false){
//    digitalWrite(TrigPin, LOW); 
//    delayMicroseconds(2);       
//    digitalWrite(TrigPin, HIGH); 
//    delayMicroseconds(10); 
//    digitalWrite(TrigPin, LOW); //Control the emission of ultrasound
//    mm = pulseIn(EchoPin, HIGH) / 58.0; 
//    mm_int = int(mm*10);
//    // the distance must be between 0~200mm. because the sensor may occur some mistake, add a function to check the validty of the value
//    if (mm_int<300){
//      valid_value = true; 
//    }
//  }
//
//
////print the distance in LCD1602
//  lcd.setCursor(0, 0);
//  lcd.print("Abstand:");
//  lcd.print(mm_int);
//  lcd.print("mm  ");
//  
//  return mm_int;    
//  }



int getUltrasonicDistance()
{
//  const int TrigPin = D0; //Send out ultrasound
//  const int EchoPin = D1; //Received reflected echoes
  
  const int TrigPin = 23; //Send out ultrasound
  const int EchoPin = 22; //Received reflected echoes
  pinMode(TrigPin, OUTPUT); 
  pinMode(EchoPin, INPUT); 

  
  float cm; //distance
  int cm_int;
  digitalWrite(TrigPin, LOW); 
  delayMicroseconds(2);       
  digitalWrite(TrigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TrigPin, LOW); //Control the emission of ultrasound
  cm = pulseIn(EchoPin, HIGH) / 58.0; 
  //cm_int = int(cm*10);
  cm_int = random(45,60); 
//  Serial.print("Distance:"); 
//  Serial.print(cm_int); 
//  Serial.print("cm   "); 
//  Serial.println();

//print the distance in LCD1602
  lcd.setCursor(0, 0);
  lcd.print("Abstand:");
  lcd.print(cm_int);
  lcd.print("mm  ");

  #ifdef TIMING
      timingArray[8] = cm_int; //timingArray[8] stores the distance
  #endif
      
  return cm_int;    
  }
