/*
   #################################
   ## Ethereum Transaction Issuer ##
   #################################

   Configuration file.

   Author: David Michel
*/

#ifndef CONFIG_H
#define CONFIG_H

static const int STILL_DISTANCE = 56;
static const int TOLERANCE_DISTANCE = 8;
static const int WAIT_BETWEEN_GET_DISTANCE_MS = 250
/*
  ------------------------------------------------------------------------------------------------------
  Values that should not be changed by the user.
  ------------------------------------------------------------------------------------------------------
*/
static const char GET_NONCE[] = "eth_getTransactionCount";
static const char SEND_TRANSACTION[] = "eth_sendRawTransaction";

static const char HTTP_HOST[] = "api-ropsten.etherscan.io";  //api-ropsten.etherscan.io
static const char API_KEY[] = "J2CQ6WY8SG78VBGDJCT6UBKRM2CM7B4TS2"; //J2CQ6WY8SG78VBGDJCT6UBKRM2CM7B4TS2    NJKI9XPSV45W5N2XPMAP7VK6ERG42IW6FT
static const int HTTPS_PORT = 443;   //443

// SHA1 fingerprint of Etherscan's certificate. This is TLS version 1.3
const char* HTTPS_FINGERPRINT = "b4 ba 86 36 b5 28 4b c8 26 db 00 d7 69 eb 69 a0 8b 8c 21 27";  //cc 80 51 b1 1e 83 02 b1 2e 12 78 02 30 52 05 27 ef 56 05 30
                                       

/*
  ------------------------------------------------------------------------------------------------------
  Values that can be adapted by the user. a84966762035f9c1e31541a659aa0e5eb104ae5ceb42bf761ad5bab7084fcd5e
  ------------------------------------------------------------------------------------------------------
*/
//EXAMPLE: static const char PRIVATE_KEY[] = "a32f7947e16510697ebf09fcb064674b33c1fc08dd2eda11906a56809f05a552";  //A84966762035F9C1E31541A659AA0E5EB104AE5CEB42BF761AD5BAB7084FCD5E
static const char PRIVATE_KEY[] = "A84966762035F9C1E31541A659AA0E5EB104AE5CEB42BF761AD5BAB7084FCD5E"; // a84966762035f9c1e31541a659aa0e5eb104ae5ceb42bf761ad5bab7084fcd5e
static const int PRIVATE_KEY_LENGTH = 64;

//EXAMPLE static const char PUBLIC_KEY[] = "";  9cea246718cda5dc32b6cf23c8e201bd0f9a1a2c857c82de106de7427271d2deb8f266e0037cf7b6fd9fac23b3a0bb4eef854b7f2db438db7d00a9bbdd808f7d
static const char PUBLIC_KEY[] = "";
static const int PUBLIC_KEY_LENGTH = 128;

//EXAMPLE static const char ACCOUNT_ADDRESS[]= "0x076Fc48A5B6c408C7893875E8Fe1F6B09067D180";  //0x7d3b7B73e2F96161d156f1826367B2B5560D7FaC
static const char ACCOUNT_ADDRESS[] = "0x7d3b7B73e2F96161d156f1826367B2B5560D7FaC"; //0x076Fc48A5B6c408C7893875E8Fe1F6B09067D180  0x954229EfBa7d6712434Aa5e07741b890f56A7591

// Wifi ID and Password
static const char WIFI[] = "Liamchen001";  //
static const char PASSWORD[] = "1234567890";

//Transaction interval
static const int WAIT_BETWEEN_TRANSACTIONS_MS = 1;  //10000
static const int MAXIMUM_AMOUNT_TRANSACTIONS = 30;

/*
  ------------------------------------------------------------------------------------------------------
  Transaction Parameters
  ------------------------------------------------------------------------------------------------------
*/
static const char* CHAIN_ID = "0x03";
static const char* GAS_PRICE = "0x3b9aca00";
static const char* GAS_LIMIT = "0xF4240";

// This is the smart contract address.
static const char* RECEIVING_ADDRESS = "0xeFA1DB1275480c1aaf6f8FBF36Ed277822518834";

// This is the function ABI with a placeholdder ARG1 which is replaced by the actual parameter at runtime.
static const char* TRANSACTION_DATA_1 = "0x1b5a8300ARG1";  //0dbd5e81ARG1   0dc1873fARG1   0x1b5a83ARG1
static const char* TRANSACTION_DATA_2 = "0xdd7b24740000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000fARG1";

//This is the ABI of function in Smart Contract type 1   
static const char* CONTRACT_GET_SIGNAL_1 = "0xe3fb74c600000000000000000000000000000000000000000000000000000000000000380000000000000000000000000000000000000000000000000000000000000005";   //"Get_Signal_1"   0x6bddab8b
static const char* CONTRACT_GET_LAST_VALUE_1 = "0x732faf8d0000000000000000000000000000000000000000000000000000000000000001"; //"Get_Last_i_Value_1(uint i)"
static const char* CONTRACT_GET_LAST_VALUE_2 = "0xdecb59e40000000000000000000000000000000000000000000000000000000000000001"; //"Get_Last_i_Value_2(uint i)"


// Contains the recovery parameter for the Ropsten test net depending on the recovery bit.
static const char* REC_ID_FALSE = "0x29";
static const char* REC_ID_TRUE = "0x2a";

#endif // CONFIG_H
