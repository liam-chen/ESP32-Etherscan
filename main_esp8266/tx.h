/*
   #################################
   ## Ethereum Transaction Issuer ##
   #################################

   Structure of an Ethereum transaction.
   
   Author: David Michel
*/

#ifndef TX_H
#define TX_H

#include <string>

class tx
{
  public:
    std::string nonce;
    std::string gasPrice;
    std::string gasLimit;
    std::string to;
    std::string value;
    std::string data;
    std::string v;
    std::string r;
    std::string s;
};

#endif // TX_H
