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
  //int value = 20;
  int value = getUltrasonicDistance();
  String output = "0x" + intToHex(value);
  Serial.println("\nDistance:");
  Serial.print(value);
  Serial.println("mm");
  return output;
}

int getUltrasonicDistance()
{
  const int TrigPin = D0; //Send out ultrasound
  const int EchoPin = D1; //Received reflected echoes
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
  cm_int = int(cm*10); 

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
