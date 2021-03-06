/*
   #################################
   ## Ethereum Transaction Issuer ##
   #################################

   Main file of the Ethereum Transaction Issuer.
   This component periodically builds Ethereum transactions and sends them via the Etherscan API.
   It calls functions from the other components and coordinates the building process.
   If necessary, a flag for measuring the execution time can be set.
   Output is written the the Serial Bus.
   Make sure to check the config file.
   
   Author: David Michel
*/

#include <stdint.h>
#include <uECC.h>
#include "keccak.h"
#include "tx.h"
#include "config.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(D8, D7, D6, D5, D4, D3); //for ESP8266


extern "C" {

  // Function to generate random noise for uECC -RNG obtained from analog pin.
  int RNG(uint8_t *dest, unsigned size) {
    while (size) {
      uint8_t val = 0;
      for (unsigned i = 0; i < 8; ++i) {
        //int init = analogRead(A0);
        int init = random(1023);
        int count = 0;
        while (random(1023) == init) {  //changed from "analogRead(A0) == init"
          ++count;
        }      
        if (count == 0) {
          val = (val << 1) | (init & 0x01);
        } 
        else {
          val = (val << 1) | (count & 0x01);
        }
      }
      *dest = val;
      ++dest;
      --size;
    }
    return 1;
  }

}





// Preprocessor flag for timing analysis. Comment out to deactivate timing analysis.
// #define TIMING

tx transaction;
Keccak keccak;
String sInitialTransactionNonceHex = "";
int iTransactionCounter = 0;
//boolean runLoop = true;
boolean variate = false;  //true: type1- only single value is sent to SC and SC judge if it is still.
                         //false: type2- value array of distance is sent to SC
//int Tolerence = 1; // tolerence for the distance


#ifdef TIMING
  unsigned long timingArray[9];
  unsigned long startTime;
//  unsigned long oneIterationTime;
#endif

/*
   Setup function is called once when MCU gets power.
*/
void setup() {
  Serial.begin(115200);
  // Set previously defined function for random number.
  uECC_set_rng(&RNG);
  
  // Configure LED to indicate sucessful transaction.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  pinMode(3, OUTPUT);  // pin 3 is for solenoid's moving
  digitalWrite(3,LOW); 
  pinMode(D2, INPUT_PULLUP); //D2 is for taster. when the taster is pressed, D2 is LOW
  
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("WLAN???");
  lcd.clear();
  
  #ifdef TIMING
  startTime = millis();
  #endif
   
  setupWifi();
  
  #ifdef TIMING
  timingArray[0] = millis () - startTime;  
  startTime = millis();
  #endif
  
  
  

  // Calculate the initial Ethereum account nonce.
  sInitialTransactionNonceHex = sendGetRequestNonce(ACCOUNT_ADDRESS);

  #ifdef TIMING
  timingArray[1] = millis () - startTime;  
  startTime = millis();
  #endif
  
  //delay(3000);
}



/*
   Loop function is called periodically.
   Here, the transaction building process gets coordinated.
*/


void loop() { 
  String sContractSignal = "";
  int intContractSignal = 0;
  int loop_number = 0; //loop number for type1
  int TasterVal = digitalRead(D2);// if button is pressed, then change the type.
  int still_loop = 0;
  boolean TransactionSuccess = true;
  Serial.println("Start!!");
  
  while (variate == false){
    lcd.setCursor(15,1);
    lcd.print("1");
    loop_number++;
    //Serial.print("loop:");
    //Serial.println(loop_number);           
    
    //print the loop number in LCD1602
    lcd.setCursor(13, 1);
    lcd.print(loop_number);
    
    //Serial.println("\ntype1");
    
    String _Transaction="";       
      
    // Initialize transaction values.
    setTransactionDetail_1();

    // Set transaction nonce.
    transaction.nonce = getTransactionNonce(sInitialTransactionNonceHex).c_str();
   
    //Serial.print(transaction.nonce);
    //Serial.println("\nSTART OF TRANSACTION BUILDING");
      
    // Replace new transaction data with actual sensor value, only when the last transaction succeeded.
    if(TransactionSuccess == true){
      setTransactionData_1(removeHexFormatting(getAnalogInputHex()), "ARG1");
    }
      
     #ifdef TIMING
     timingArray[2] = millis () - startTime;
     timingArray[3] = 0;  
     startTime = millis();
     #endif
      
    if(transaction.nonce != "invalid"){
      // Generate signed transaction and send it to Etherscan API.
      _Transaction=generateTransaction();

        #ifdef TIMING
        timingArray[4] = millis () - startTime;  
        startTime = millis();
        #endif

      // if the transaction was not sent successfully
      if(sendGetRequestTransaction(_Transaction) == false){
        TransactionSuccess = false;
                
        #ifdef TIMING 
        for(int i = 0; i < sizeof(timingArray)/sizeof(unsigned long); i++)
        {
          Serial.print(timingArray[i]);
          Serial.print(",");
        }
        //Serial.println("\n");
        Serial.println(intContractSignal);
        startTime = millis();
        #endif
        continue;
      }
      else{
          TransactionSuccess = true;
      }
    }
    else{
      Serial.println("Transaction nonce could not be calculated correctly. No transaction was built");
    }

    //when loop>4, get the signal from samrt contract, if the object still is.
    if (loop_number>2){
      sContractSignal = sendGetContractSignal(RECEIVING_ADDRESS);
      //Serial.println(sContractSignal);
      intContractSignal = strtol(removeHexFormatting(sContractSignal).c_str(), NULL, 16);

        #ifdef TIMING
        timingArray[6] = millis () - startTime;  
        startTime = millis();
        #endif
        
      //Serial.print("whether the object is still:");
      //Serial.println(intContractSignal);
      if (intContractSignal != 0){
        //Serial.println("still!");
        lcd.setCursor(14,0);
        lcd.print(" S");
        //Serial.println("Hub beweget.");
        Hub_Move_1();
      }
      else {
        //Serial.println("not still!");
        lcd.setCursor(14,0);
        lcd.print("NS");          
      }
      
        #ifdef TIMING
        timingArray[7] = millis () - startTime;  
        startTime = millis();
        #endif
    }

    //if the taster is pressed, then change the type 
    TasterVal = digitalRead(D2);
    if (TasterVal == LOW) {
      changeVariate();
    }
    if (variate == true){
      break;
    }
      
    // Wait for specified time gap until next transaction.
    delay(WAIT_BETWEEN_TRANSACTIONS_MS);
      
      #ifdef TIMING 
        //Serial.println("\n----------------TIMING ANALYSIS Start--------------\n");
        for(int i = 0; i < sizeof(timingArray)/sizeof(unsigned long); i++)
        {
          Serial.print(timingArray[i]);
          Serial.print(",");
        }
        Serial.println(intContractSignal);
        startTime = millis();
        //Serial.println("\n----------------TIMING ANALYSIS END----------------\n");
      #endif
  }

 
  while ( variate == true ){
    still_loop = 0;
    
    lcd.clear();
    lcd.setCursor(15,1);
    lcd.print("2"); 
    
    //Serial.println("\ntype2");    
    
    String _Transaction="";
    int DataInTransction[15];
    String DataInTransction_hex ="";
    boolean Still_old = false;
    boolean Still_new = false;     
    unsigned long Time_1;
    unsigned long Time_2;
    
    for (int n=0; n<15; n++){
      //Serial.print("n:");
      //Serial.println(n);
      Time_1 = millis();
      int MoveMax = getUltrasonicDistance();
      int MoveMin = MoveMax;
      int value[250];
      for (int i=0; i<=250; i++){
        
        value[i] = getUltrasonicDistance();
      
        //Serial.print(value[i]);
        //Serial.print(",");
                    
        //when loop>1, judge if the object is still
        if (i>5){
          int value_aktuell = value[i-2];
          int Distance_still = 56;  // still distance 
          int diff = 8; //tolerance for still distace
          MoveMax = max(MoveMax, value_aktuell);
          MoveMin = min(MoveMin, value_aktuell);  
          if (Get_absolute_value(Distance_still-value[i-1])<diff&&Get_absolute_value(Distance_still-value[i-2])<diff&&Get_absolute_value(Distance_still-value[i-3])<diff&&Get_absolute_value(Distance_still-value[i-4])<diff&&Get_absolute_value(Distance_still-value[i-5])<diff){
            Still_new = 1;
            still_loop++;
            lcd.setCursor(0,1);
            lcd.print("still! ");
            lcd.print(still_loop);
            lcd.print("/6 ");
            //Serial.print(i);
            //Serial.println("Still!");
            //StillDistance = value[i];  
          }
         else{
            Still_new = 0;
            lcd.setCursor(0,1);
            lcd.print("nicht still!");
            //Serial.print(i); 
            //Serial.println("nicht Still!");
            still_loop = 0;         
          }       
        }
        if (Still_old !=Still_new){
          Still_old = Still_new;
          break;
        }
        if(still_loop>5){
          Hub_Move_2();
        }
        else{
          delay(250);
        }
        //delay(500);
        
      }
      //Serial.println("\n--------------------------------\n");
      
        DataInTransction[n] = MoveMin;
        DataInTransction_hex = DataInTransction_hex + patch64bit(intToHex(DataInTransction[n]));
        n++;
        DataInTransction[n] = MoveMax;
        DataInTransction_hex = DataInTransction_hex + patch64bit(intToHex(DataInTransction[n]));
        n++;
        DataInTransction[n] = int(millis() - Time_1);
        DataInTransction_hex = DataInTransction_hex + patch64bit(intToHex(DataInTransction[n]));
    }

    #ifdef TIMING
        timingArray[2] = millis () - startTime;  
        startTime = millis();
    #endif
    
    for (int i = 0; i < 15; i++) {
        //Serial.print(DataInTransction[i]);
        //Serial.print(",");
    }
    //Serial.println(); 
    // Initialize transaction values.
    setTransactionDetail_2();

    // Set transaction nonce.
    transaction.nonce = getTransactionNonce(sInitialTransactionNonceHex).c_str();
    //Serial.print(transaction.nonce);

    //Serial.println("\nSTART OF TRANSACTION BUILDING");
      
    // Replace transaction data placeholder with actual sensor value.
    setTransactionData_2(DataInTransction_hex, "ARG1");

    #ifdef TIMING
        timingArray[3] = millis () - startTime;  
        startTime = millis();
    #endif
      
    if(transaction.nonce != "invalid"){
      // Generate signed transaction and send it to Etherscan API.
      _Transaction=generateTransaction();

      #ifdef TIMING
        timingArray[4] = millis () - startTime;  
        startTime = millis();
      #endif
    
      while(sendGetRequestTransaction(_Transaction) == false){
        _Transaction=generateTransaction();
        #ifdef TIMING 
        //Serial.println("\n----------------TIMING ANALYSIS Start--------------\n");
        for(int i = 0; i < sizeof(timingArray)/sizeof(unsigned long); i++)
        {
          Serial.print(timingArray[i]);
          Serial.print(",");
        }
        //Serial.println("\n");
        Serial.println(intContractSignal);
        startTime = millis();
        //Serial.println("\n----------------TIMING ANALYSIS END----------------\n");
        #endif
        
        break;
      }
    }
    else
    {
        //Serial.println("Transaction nonce could not be calculated correctly. No transaction was built");
    }
    
    //switch to another type
    TasterVal = digitalRead(D2);
    
    #ifdef TIMING 
        //Serial.println("\n----------------TIMING ANALYSIS Start--------------\n");
        for(int i = 0; i < sizeof(timingArray)/sizeof(unsigned long); i++)
        {
          Serial.print(timingArray[i]);
          Serial.print(",");
        }
        //Serial.println("\n");
        Serial.println(intContractSignal);
        startTime = millis();
        //Serial.println("\n----------------TIMING ANALYSIS END----------------\n");
    #endif
      
    if (TasterVal == LOW) {
        changeVariate();
    }
    if (variate == false){
        break;
    }
    
  }
  

  
}

/*
  Generates a signed transaction for unsigned transaction information.
  @return The string containing the RLP encoded signed transaction.
*/
String generateTransaction()
{
  lcd.setCursor(0,1);  
  lcd.print("Transaktion!");
  // Specify ECDSA curve type
  const struct uECC_Curve_t * curve = uECC_secp256k1();
  
  String sUnsginedTransaction = "";
  String sUnsignedHash = "";
  std::string cstrUnsignedHash = "";
  boolean oddCounter;
  boolean signatureValid = 0;
  uint8_t byteHashInt[32] ={};
  uint8_t bytePrivKey[32] = {};
  uint8_t byte_r[32] = {0};
  uint8_t byte_s[32] = {0};
  uint8_t sigRes[64] = {0};
  int iEccWasSuccess = 0;
    
  sUnsginedTransaction = RlpEncodeTransaction(transaction);
  //Serial.println("\nRLP of unsigned transaction:");
  //Serial.println(sUnsginedTransaction);
  
//  #ifdef TIMING
//    timingArray[0] = millis () - startTime;  //time for Acquire Data and Build Unsigned Transaction RLP
//    startTime = millis();
//  #endif
  
  oddCounter = false;
  byte byteUnsignedTransaction[sUnsginedTransaction.length()/2]; 

  // Loop for transformation of hex RLP to bytes. Result is stored in byteUnsignedTransaction.  because 2 hex into 1 bytes
  for(int i = 0; i < sUnsginedTransaction.length(); i++)
  {
    if (oddCounter)
      {
        String currentSubstr = sUnsginedTransaction.substring(i - 1, i + 1);
        byte currentByte = (byte)byteFromTwoHex(currentSubstr);
        byteUnsignedTransaction[i/2] = currentByte;
      }
      oddCounter =! oddCounter;
  }

  cstrUnsignedHash = keccak(byteUnsignedTransaction, sizeof(byteUnsignedTransaction));
  //Serial.println("\nResult of Keccak execution as hex values is:");
  //Serial.println(cstrUnsignedHash.c_str());

//  #ifdef TIMING
//    timingArray[1] = millis() - startTime;   // time for hash
//    startTime = millis();
//  #endif

  sUnsignedHash = cstrUnsignedHash.c_str();
  
  oddCounter = false;

  // Loop for transformation of hex encoded hash to bytes. Result is stored in byteHashInt.
  for(int i = 0; i <= sUnsignedHash.length() - 1; i++)
  {
    if (oddCounter)
    {
      String currentSubstr = sUnsignedHash.substring(i - 1, i + 1);
      uint8_t currentByte = (uint8_t)byteFromTwoHex(currentSubstr);
      byteHashInt[i/2] = currentByte;
    }
    oddCounter =! oddCounter;      
  }

  // Loop for transformation of hex encoded Private Key to bytes. Result is stored in bytePrivKey.
  oddCounter = false;
  for(int i = 0; i < PRIVATE_KEY_LENGTH; i++)
  {
    if (oddCounter)
    {
      String currentSubstr = String(PRIVATE_KEY[i-1]) + String(PRIVATE_KEY[i]);
      uint8_t currentByte = (uint8_t)byteFromTwoHex(currentSubstr);
      bytePrivKey[i/2] = currentByte;
    }
    oddCounter =! oddCounter;        
  }

  // The recovery Bit is stored here
  uint8_t recid[1] = {2};
  
  //Serial.println("\nNow signing the transaction starts.");
  
  // Try signing maximum ten times
  for(int i = 0; i < 10 && signatureValid == 0; i++)
  {
    iEccWasSuccess = uECC_sign(bytePrivKey, byteHashInt, sizeof(byteHashInt), sigRes, curve, recid);
    //iEccWasSuccess = uECC_sign(bytePrivKey, byteHashInt, sizeof(byteHashInt), sigRes, curve);
     delay(100);   //100
    // Check MSB smaller than half the maximum value. Otherwise, the transaction will be rejected by the Ethereum network.
    if(sigRes[32] < 128)
    {
      //Serial.println("Signature was VALID!");
      signatureValid = 1;
    }
    else
    {
      //Serial.println("Signature was INVALID! - repeat");
      signatureValid = 0;
      delay(100);
    }
  }  

  if(iEccWasSuccess == 1)
  {
    //Serial.println("Signing was SUCCESSFUL");  
    //Serial.println("\Recovery bit is:");
    //Serial.println(recid[0]);  
  }
  else
  {
    //Serial.println("Signing FAILED"); 
    return "invalid"; 
  }

  // Loop to split 64 byte signature result to r and s
  for(int i = 0; i < sizeof(sigRes); i++)
  {
    if(i < 32)
    {
      byte_r[i] = sigRes[i];
    }
    else
    {
      byte_s[i-32] = sigRes[i];
    }
  }
  // Set r, s and v value of transaction
  transaction.r = "0x";
  transaction.r += bytesToHex(byte_r, sizeof(byte_r)).c_str();
  transaction.s = "0x";
  transaction.s += bytesToHex(byte_s, sizeof(byte_s)).c_str();

  if(recid[0] == 0)
  {
    // Value from config for recovery ID of current Ethereum blockchain instance.
    transaction.v = REC_ID_FALSE;
  }
  else
  {
    transaction.v = REC_ID_TRUE;
  }

  //Serial.println("\nr of ECDSA is:");
  //Serial.println(transaction.r.c_str());
  //Serial.println("\ns of ECDSA is:");
  //Serial.println(transaction.s.c_str());  
  
  String signedTransaction = RlpEncodeTransaction(transaction);
  //Serial.println("\n--------------------------------------------------------\nFinal transaction is:");
  //Serial.println("0x" + signedTransaction);

//  #ifdef TIMING
//    timingArray[3] = millis() - startTime;
//    startTime = millis();
//  #endif

  return signedTransaction;
}


/*
  Sets the values for an unsigned transaction.
  As the transaction is currently sent to a smart contract, the value is 0.
*/
void setTransactionDetail_1()
{
  transaction.gasPrice = GAS_PRICE;
  transaction.gasLimit = GAS_LIMIT;
  transaction.to = RECEIVING_ADDRESS;
  transaction.value = "";  
  //transaction.data = TRANSACTION_DATA_1;
  transaction.v = CHAIN_ID;
  transaction.r="";
  transaction.s="";  
}

void setTransactionDetail_2()
{
  transaction.gasPrice = GAS_PRICE;
  transaction.gasLimit = GAS_LIMIT;
  transaction.to = RECEIVING_ADDRESS;
  transaction.value = "";  
  transaction.data = TRANSACTION_DATA_2;
  transaction.v = CHAIN_ID;
  transaction.r="";
  transaction.s="";  
}

/*
  Generates an ecdsa key pair.
  The keys are printed on the serial output.
*/
void generateKeyPair()
{
  uint8_t priv[32] = {};
  uint8_t pub[64] = {};
  const struct uECC_Curve_t * curve = uECC_secp256k1();
  int keyGenSuccess = uECC_make_key(pub, priv, curve);
  String strPriv = bytesToHex(priv, sizeof(priv));
  String strPub = bytesToHex(pub, sizeof(pub));
  //Serial.println("\nprivate key is:");
  //Serial.println(strPriv);
  //Serial.println("\npublic key is:");
  //Serial.println(strPub);
}


/*
  Calculates the current transaction nonce based on the intial nonce at startup and the amount of sent transactions since then.
  @return The hex prefixed string containing the nonce of the next transaction
*/
String getTransactionNonce(String initial)
{
  if(initial != "invalid")
  {
    return "0x" + String(intToHex(strtol(removeHexFormatting(initial).c_str(), NULL, 16) + iTransactionCounter));
  }
}


/*
  Sets the data of the transaction.
  A placeholder can be specified which is then replaced with an actual value at runtime.
  As the data is sent to a smart contract, it gets padded to 32 bytes with leading zeros.
*/
void setTransactionData_1(String input, String dummy)
{
  transaction.data = TRANSACTION_DATA_1;
  String output = "";
  for (int i = 0; i < 64 - input.length(); i++)
  {
    output += "0";
  }
  output += input;

  String transactionData = transaction.data.c_str();
  transactionData.replace(dummy, output);
  Serial.println(transactionData);
  transaction.data = transactionData.c_str();
}

void setTransactionData_2(String input, String dummy)
{
  String transactionData = transaction.data.c_str();
  transactionData.replace(dummy, input);
  //Serial.println(transactionData.c_str());
  transaction.data = transactionData.c_str();
}



String patch64bit (String input){
  String output = "";
  for (int i = 0; i < 64 - input.length(); i++)
  {
    output += "0";
  }
  output += input;
  return output;
}

void changeVariate(){
  if (variate == true){
    variate = false;
  }
  else {
    variate = true; 
  }
}

int Get_absolute_value(int a){
   if (a>0) return uint(a);
   else return uint(-a);
}

void Hub_Move_2(){
   // pin3 is RX. RX is connected with the solenoid
  digitalWrite(3,HIGH);
  delay(70);
  digitalWrite(3,LOW);
//  delay(100);
//  digitalWrite(3,HIGH);
//  delay(50);
//  digitalWrite(3,LOW);
}


void Hub_Move_1(){
   // pin3 is RX. RX is connected with the solenoid
  digitalWrite(3,HIGH);
  delay(160);
  digitalWrite(3,LOW);

}
