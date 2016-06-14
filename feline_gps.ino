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

SoftwareSerial mySerial(10, 16); // RX, TX

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

  // Start GPS
  gpsPowerOn(&mySerial);
  //mySerial.write("AT+CGNSPWR=1\n");
  delay(1000);
  //mySerial.write("AT+CGNSTST=1\n");
}

void loop() // run over and over
{
  /*
  if (Serial.available()) {
    switch(Serial.read()) {
      case 'r':
         Serial.println(F("hola"));
         gpsPowerOn(mySerial);
        break;
    }
  } 
 */

/*
  if (mySerial.available())
    Serial.write(mySerial.read()); 
  */
  
  Point * point = gpsGetPoint(&mySerial);
  
  if (point != NULL) {
    Serial.println(point->ggaLatitude);
    Serial.println(point->ggaLongitude);
    Serial.println(point->accuracy);
    Serial.println(point->altitude);
  } else {
    Serial.println(F("point is null")); 
  }
  
  /*
  if (mySerial.available())
    Serial.write(mySerial.read());
    
  if (Serial.available())
  { 
    while(Serial.available())
    {
      mySerial.write(Serial.read());
    }
    mySerial.println();
  }
  */
  
}


