/*
   #################################
   ## Ethereum Transaction Issuer ##
   #################################

   This file contains the RLP operations necessary to build a valid Ethereum transaction.
   There are also some helper functions as well as conversions from different data types.
   
   Author: David Michel
*/


/*
  Calculates the RLP encoding of a single string.
  @return RLP encoded string.
*/
String RlpEncodeString(String input)
{
  String output = "";
  // Pad with leading zero in case input has uneven length.
  if(input.length() % 2 == 1)
  {
    output = "0" + input;
  }
  else
  {
    output = input;
  }
  // Check for empty string as input.
  if (output == "")
  {
    return "80";
  }
  // Check if input was single character.
  else if (output.length() <= 2 && (unsigned char)byteFromTwoHex(output) < 128)
  {
    return output;  
  }
  // If the input is more than one char, encode the string's length.
  else
  {
    // Take half here, as 2 hex values are 1 byte.
    return encodeLength(output.length()/2, 128) + output; 
  }
}


/*
  Calculates the RLP encoding of a transaction object.
  @param pTransaction The transaction that is to be RLP encoded
  @return RLP encoded transaction as a string.
*/
String RlpEncodeTransaction(tx pTransaction)
{
  // Perform RLP Encoding of single transaction strings and concatenate the result.
  String serializedTransaction =  hexToRLPEncode(pTransaction.nonce.c_str()) + 
                                  hexToRLPEncode(pTransaction.gasPrice.c_str()) + 
                                  hexToRLPEncode(pTransaction.gasLimit.c_str()) + 
                                  hexToRLPEncode(pTransaction.to.c_str()) + 
                                  hexToRLPEncode(pTransaction.value.c_str()) + 
                                  hexToRLPEncode(pTransaction.data.c_str()) +
                                  hexToRLPEncode(pTransaction.v.c_str())+
                                  hexToRLPEncode(pTransaction.r.c_str())+
                                  hexToRLPEncode(pTransaction.s.c_str());

  // Encode the length of the resulting string.
  return encodeLength(serializedTransaction.length()/2, 192) + serializedTransaction;
}


/*
  Encodes the string length according to the RLP rules.
  @param inputLength The amount of bytes as an integer
  @param offset An offset that is added to the length.
  @return Length in bytes as an RLP encoded string.
*/
String encodeLength(int inputLength, int offset)
{
  if (inputLength < 56)
  {
    // Perform length encoding with one part indicating length.
    return intToHex(inputLength + offset);;
  }
  else
  {
    // Perform length encoding with two parts indicating length.
    String hexLength = intToHex(inputLength);
    int tailLength = hexLength.length() / 2;
    String lengthEncoding = intToHex(offset + 55 + tailLength);
    return lengthEncoding + hexLength;
  }
}


/*
  Calculates a hex encoded string from an integer.
  @param input The integer value to be transformed.
  @return Hex encoded string of input integer.
*/
String intToHex(int input)
{
  String output = "";
  String intermediate = "";
  if(input < 16 && input > -1)
  {
    output = "0";
    output += String(input, HEX);
    return output;
  }
  else
  {
    intermediate = String(input, HEX);
    if (intermediate.length() % 2 == 1)
    {
      // Pad with leading zero if input results in uneven hex.
      output = "0" + intermediate;      
    }
    else 
    {
      output = intermediate;
    }
    return output;
  }
}


/*
  Calculates a byte value from two hex values.
  @param input The string containing hex values.
  @return A char containing the byte of the two hex values.
*/
char byteFromTwoHex(String input)
{
  unsigned long number = strtoul(input.c_str(), NULL, 16);
  return (unsigned char)number;
}


/*
  Gets a hex formatted string, calls removeHexFormatting and then RlpEncodeString.
  @param input A hex formatted string.
  @return Result of RlpEncodeString function call.
*/
String hexToRLPEncode(String input)
{
  input = removeHexFormatting(input);
  return(RlpEncodeString(input));
}


/*
  Removes the hex prefix 0x of a string.
  @param input A hex formatted string.
  @return The input string with the 0x part cut away.
*/
String removeHexFormatting(String input)
{
  if (input[0] == '0' && input[1] == 'x')
  {
    return input.substring(2, input.length());
  }
  else
  {
    return input;
  }
}


/*
  Calculates hex formatted string from a byte array.
  @param input The byte array that is to be transformed.
  @param inputSize The length of byte array.
  @return String containing hex values
*/
String bytesToHex(uint8_t input[], int inputSize)
{
  String output = "";
  static const char* lookUp = "0123456789abcdef";
  boolean even = false;
  String currentSubstr = "";
    
  for(int i = 0; i < inputSize; i++)
  {
      int firstNumber = input[i] / 16;
      int secondNumber = input[i] % 16;
      output += lookUp[firstNumber];
      output += lookUp[secondNumber];
  }
  return output;
}
