/*
   #################################
   ## Ethereum Transaction Issuer ##
   #################################

   The client handles the WiFi connection.
   It also makes HTTPS calls to the Etherscan.io API.
   
   Author: David Michel
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIICiTCCAg+gAwIBAgIQH0evqmIAcFBUTAGem2OZKjAKBggqhkjOPQQDAzCBhTEL\n" \
"MAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\n" \
"BxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMT\n" \
"IkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDgwMzA2MDAw\n" \
"MDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdy\n" \
"ZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09N\n" \
"T0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlv\n" \
"biBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQDR3svdcmCFYX7deSR\n" \
"FtSrYpn1PlILBs5BAH+X4QokPB0BBO490o0JlwzgdeT6+3eKKvUDYEs2ixYjFq0J\n" \
"cfRK9ChQtP6IHG4/bC8vCVlbpVsLM5niwz2J+Wos77LTBumjQjBAMB0GA1UdDgQW\n" \
"BBR1cacZSBm8nZ3qQUfflMRId5nTeTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/\n" \
"BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjEA7wNbeqy3eApyt4jf/7VGFAkK+qDm\n" \
"fQjGGoe9GKhzvSbKYAydzpmfz1wPMOG+FDHqAjAU9JM8SaczepBGR7NjfRObTrdv\n" \
"GDeAU/7dIOA1mjbRxwG55tzd8/8dLDoWV9mSOdY=\n" \
"-----END CERTIFICATE-----\n";


String url = "https://api-ropsten.etherscan.io/api";


// Use WiFiClientSecure class to create TLS connection
HTTPClient http;
const size_t MAX_CONTENT_SIZE = 2000; 
char response[MAX_CONTENT_SIZE];
/*
   Establishes a WiFI connection to an SSID specified in the config.
*/
void setupWifi ()
{
  Serial.println("\nConnecting to WiFi: ");
  Serial.println(WIFI);
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
//void establishHttpsConnection() 
//{
//  Serial.print("\nConnecting to the host address:");
//  Serial.println(HTTP_HOST);
//  client.setFingerprint(HTTPS_FINGERPRINT);
//  if (!client.connect(HTTP_HOST, HTTPS_PORT))   // if Connection failed, how to reconnect aotumatically?
//  {
//    Serial.println("Connection failed.");
//    return;
//  }
//}


/*
   Sends a HTTPS GET request for issuing a signed transaction to the Etherscan API.
   @param transaction The signed transaction RLP as a string.
*/
boolean sendGetRequestTransaction(String transaction)
{
  if(transaction != "invalid")
  {
  
//    #ifdef TIMING
//      timingArray[5] = millis() - startTime;
//      startTime = millis();
//    #endif
  
    http.begin(url + "?module=proxy&action=" + SEND_TRANSACTION +"&hex=0x" + transaction + "&apikey=" + API_KEY, root_ca);
    
    int httpCode = http.GET();
    String line = http.getString();
    
    #ifdef TIMING
      timingArray[5] = millis () - startTime;
      timingArray[6] = 0;
      timingArray[7] = 0;
            
      startTime = millis();
    #endif
    if (line.indexOf("result") > 0) {
      //Serial.println("Valid response received.111");   
      Serial.print(iTransactionCounter);
      iTransactionCounter ++;     
    }
    else if (line.indexOf("error") > 0){
      lcd.setCursor(0,0);
      lcd.print("Nonce?      ");
      Serial.print(iTransactionCounter);
     
      #ifdef TIMING
      timingArray[6] = millis () - startTime;
      startTime = millis();
      #endif
      Serial.print(line);
      Serial.println(transaction);
      return false;
    }
    else 
    {
      Serial.println("No valid response received!");
    }
    //Serial.println("Response object is:");
    //Serial.println("====================");
    Serial.print(line);
    Serial.println(transaction);
    //Serial.println("====================");
    //Serial.println("The connection is closed now.");
    http.end(); //Free the resources

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
  const char * nonce = "invalid";

  http.begin(url + "?module=proxy&action=" + GET_NONCE + "&address=" + accountAddress + "&apikey=" + API_KEY, root_ca);

  //Serial.println("Request sent.");
//  while (client.connected() || client.available()) {  
//    String line = client.readStringUntil('\n');
//    //Serial.println(line);
//    if (line == "\r") 
//    {
//      //Serial.println("Headers received.");
//      break;
//    }
//  }
  int httpCode = http.GET();     
  String line = http.getString();
  
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
  http.end(); //Free the resources
  return nonce;
}

/*
   Sends a HTTPS GET request for querying Signal, Whether the object is still.
   @param contractAddress .
   @return Hex encoded Signal(bool)  true means the object is still .
*/
String sendGetContractSignal(String contractAddress)
{
  //bool Signal_Still=false;
  const char* Signal="invalid";

  http.begin(url + "?module=proxy&action=eth_call&to=" + contractAddress + "&data=" + CONTRACT_GET_SIGNAL_1 + "&apikey=" + API_KEY,  root_ca);

  //Serial.println("Request sent.");
//  while (client.connected() || client.available()) {
//    String line = client.readStringUntil('\n');
//    //Serial.println(line);
//    if (line == "\r") 
//    {
//      Serial.println("Headers received.");
//      break;
//    }
//  }
  int httpCode = http.GET();     
  String line = http.getString();
  if (line.indexOf("result") > 0) 
  {
    //Serial.println("Valid response received.");
    
    DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3));
    JsonObject& root = jsonBuffer.parseObject(line);
    if (!root.success()) 
    {
      //Serial.println("JSON parsing failed!");    
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
  //Serial.println("Response object is:");
  //Serial.println("====================");
  //Serial.println(line);
  //Serial.println("====================");
    //Serial.println("The connection is closed now.");
  http.end(); //Free the resources
  //Serial.println(Signal);
  return Signal;
}
