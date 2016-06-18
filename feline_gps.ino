//***********************************************
//************ FELINE GPS TRACKER ***************
//***********************************************
//** This is a project meant to track a route  **
//** throug GPS using the shield module SIM808 **
//** Once a track point is received and        **
//** processed, it is sent to an API web       **
//** through the SIM808 module via GPRS        **
//***********************************************
//** Date: 14/06/2016                          **
//** Author: raultov                           **
//** Email: angel.raul.tovar.martin@gmail.com  **
//***********************************************

#include <SoftwareSerial.h>
#include "gps.h"
#include "network.h"
#include "api_conf.h"

SoftwareSerial mySerial(10, 16); // RX, TX
Point point;

// User Token to access API methods
String userToken = "";

void setup()  
{
  // Initialize PIN 5 with output mode
  pinMode(5, OUTPUT);  
  
  // Hold the relay connected to PIN 5 for 3 seconds to start SIM808 module
  digitalWrite(5, HIGH);
  delay(3000);
  digitalWrite(5, LOW);  
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);
  
  delay(1000);
  
  // Enable GPRS module security
  networkSetPinCode(PHONE_PIN_CODE, &mySerial);
  delay(1000);

  // Start GPS
  gpsPowerOn(&mySerial);
  delay(1000);
  
  initializeConnection(&mySerial); 
}

void loop() { // run over and over

  while (userToken.length() == 0) {
    networkSetUserToken(&mySerial);
  }
    
  Serial.println(userToken);
  
  gpsGetPoint(&point, &mySerial);
  
  if (point.ggaLatitude.length() == 0) {
    // return null pointer when time was over and no valid point could be found
    Serial.println(F("point is null")); 
  } else {
    Serial.println(F("Point:"));
    Serial.println(point.ggaLatitude);
    Serial.println(point.ggaLongitude);
    Serial.println(point.accuracy);
    Serial.println(point.altitude);   
 
    // TODO put here a while to make sure that the point is uploaded
    bool status = uploadPoint(&point, &mySerial); 
    if (status) {
      Serial.println(F("Point uploaded successfully")); 
    } else {
      Serial.println(F("Point could not be uploaded"));
      networkSetUserToken(&mySerial);
    }
  }

  if (mySerial.available())
    Serial.write(mySerial.read()); 
  
}


