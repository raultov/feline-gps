#ifndef _NETWORK_
#define _NETWORK_1

extern String userToken;

/**
  Set pin code in order to enable GPRS module
*/
void networkSetPinCode(String pinCode, SoftwareSerial * mySerial);

/**
  Initializes tcp connection with API server
*/
void initializeConnection(SoftwareSerial * mySerial);

/**
  Set the value of user token in general variable userToken
  
  Must be called after networkSetPinCode and after initializeConnection
*/
void networkSetUserToken(SoftwareSerial * mySerial);

/**
  upload point using API rest
  
  Must be called after networkSetUserToken just to make sure that user token is available
  
  Returns true if everything was OK, otherwise returns false
*/
bool uploadPoint(Point * point, SoftwareSerial * mySerial);


#endif
