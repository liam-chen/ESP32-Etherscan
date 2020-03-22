/*
   #################################
   ## Ethereum Transaction Issuer ##
   #################################

   The client handles the WiFi connection.
   It also makes HTTPS calls to the Etherscan.io API.
   
   Author: David Michel
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>



//String url = "http://api-ropsten.etherscan.io/api";
String url = "/api";

// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;
const size_t MAX_CONTENT_SIZE = 2000; 
char response[MAX_CONTENT_SIZE];
/*
   Establishes a WiFI connection to an SSID specified in the config.
*/
void setupWifi ()
{
  Serial.println("\nConnecting to WiFi: ");
  Serial.println(WIFI);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.println("IP address is: ");
  Serial.println(WiFi.localIP());
}

/*
   Establishes an HTTPS connection to an address specified in the config.
*/
void establishHttpsConnection() 
{
  Serial.print("\nConnecting to the host address:");
  Serial.println(HTTP_HOST);
  client.setFingerprint(HTTPS_FINGERPRINT);
  if (!client.connect(HTTP_HOST, HTTPS_PORT))   // if Connection failed, how to reconnect aotumatically?
  {
    Serial.println("Connection failed!!");
    return;
  }
}


/*
   Sends a HTTPS GET request for issuing a signed transaction to the Etherscan API.
   @param transaction The signed transaction RLP as a string.
*/
boolean sendGetRequestTransaction(String transaction)
{
  if(transaction != "invalid")
  {
    establishHttpsConnection();
  
//    #ifdef TIMING
//      timingArray[5] = millis() - startTime;
//      startTime = millis();
//    #endif
  
    client.print(String("GET ") + url + "?module=proxy&action=" + SEND_TRANSACTION +"&hex=0x" + transaction + "&apikey=" + API_KEY + " HTTP/1.1\r\n" +
               "Host: " + HTTP_HOST + "\r\n" +
               "Connection: close\r\n\r\n");

    //Serial.println("Request sent.");
    while (client.connected() || client.available()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        //Serial.println("Headers received.");
        break;
      }
    }
    String line = client.readStringUntil('\n');
    
    #ifdef TIMING
      timingArray[5] = millis () - startTime;
      timingArray[6] = 0;
      timingArray[7] = 0;
            
      startTime = millis();
    #endif
    
    if (line.indexOf("result") > 0) {
      Serial.println("Valid response received.");
      // Increment counter for valid transactions since MCU startup.
      iTransactionCounter ++;     
    }
    if (line.indexOf("error") > 0){
      Serial.println("\nNonce is too small. get the new nonce\n delay 20000ms \n");
      lcd.setCursor(0,0);
      lcd.print("Nonce?      ");
      iTransactionCounter = iTransactionCounter + 1;
      //delay(3000);
      //sInitialTransactionNonceHex = sendGetRequestNonce(ACCOUNT_ADDRESS);
      
      #ifdef TIMING
      timingArray[6] = millis () - startTime;
      startTime = millis();
      #endif
      
      return false;
    }
    else 
    {
      Serial.println("No valid response received!");
    }
    Serial.println("Response object is:");
    Serial.println("====================");
    Serial.println(line);
    Serial.println("====================");
    Serial.println("The connection is closed now.");
    client.flush();//Clear the buffer

  }
  else
  {
    Serial.println("Error during transaction bulding!");
  }
  return true;
}


/*
   Sends a HTTPS GET request for querying an account'S nonce to the Etherscan API.
   @param accountAddress Public key of the account that the nonce is queried from.
   @return Hex encoded account nonce.
*/
String sendGetRequestNonce(String accountAddress)
{
  establishHttpsConnection();
  const char * nonce = "invalid";

    client.print(String("GET ") + url + "?module=proxy&action=" + GET_NONCE + "&address=" + accountAddress + "&apikey=" + API_KEY + " HTTP/1.1\r\n" +
              "Host: " + HTTP_HOST + "\r\n" +
               "Connection: close\r\n\r\n");

  //Serial.println("Request sent.");
  while (client.connected() || client.available()) {  
    String line = client.readStringUntil('\n');
    //Serial.println(line);
    if (line == "\r") 
    {
      //Serial.println("Headers received.");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.indexOf("result") > 0) 
  {
    Serial.println("Valid response received.");
    
    DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3));
    JsonObject& root = jsonBuffer.parseObject(line);
    if (!root.success()) 
    {
      Serial.println("JSON parsing failed!");    
    }
    else
    {
      nonce = root["result"];
    }    
  }   
  else 
  {
    Serial.println("No valid response received!");
  }
  Serial.println("Response object is:");
  Serial.println("====================");
  Serial.println(line);
  Serial.println("====================");
  Serial.println("The connection is closed now.");
  client.flush();//Clear the buffer
  return nonce;
}

/*
   Sends a HTTPS GET request for querying Signal, Whether the object is still.
   @param contractAddress .
   @return Hex encoded Signal(bool)  true means the object is still .
*/
String sendGetContractSignal(String contractAddress)
{
  establishHttpsConnection();
  //bool Signal_Still=false;
  const char* Signal="invalid";

  client.print(String("GET ") + url + "?module=proxy&action=eth_call&to=" + contractAddress + "&data=" + CONTRACT_GET_SIGNAL_1 + "&apikey=" + API_KEY + " HTTP/1.1\r\n" +
              "Host: " + HTTP_HOST + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent.");
  while (client.connected() || client.available()) {
    String line = client.readStringUntil('\n');
    //Serial.println(line);
    if (line == "\r") 
    {
      Serial.println("Headers received.");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.indexOf("result") > 0) 
  {
    Serial.println("Valid response received.");
    
    DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3));
    JsonObject& root = jsonBuffer.parseObject(line);
    if (!root.success()) 
    {
      Serial.println("JSON parsing failed!");    
    }
    else
    {
      Signal = root["result"];
    }    
  }   
  else 
  {
    Serial.println("No valid response received!");
  }
  Serial.println("Response object is:");
  Serial.println("====================");
  Serial.println(line);
  Serial.println("====================");
  Serial.println("The connection is closed now.");
  client.flush();//Clear the buffer
  //Serial.println(Signal);
  return Signal;
}
