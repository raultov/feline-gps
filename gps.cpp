#include <Arduino.h>
#include <SoftwareSerial.h>
#include "gps.h"
#include "api_conf.h"

#define DATA_TIMEOUT 120000L
#define GPGGA "GPGGA"

String gpsChecksumNMEA0183(const String s) {
    int c = 0;
    int i = 0;
    
    while (i < s.length()) {
      c ^= s.charAt(i);
      i++; 
    }

    String string = String(c, HEX);
    string.toUpperCase();
    return string;
}

void gpsPowerOn(SoftwareSerial * mySerial) {
  mySerial->println("AT+CGNSPWR=1");
}

void gpsPowerOff(SoftwareSerial * mySerial) {
  mySerial->println("AT+CGNSPWR=0");
}

void gpsGetPoint(Point * point, SoftwareSerial * mySerial) {
  bool found = false;
  String string = String(GPGGA);
  
  // reset point
  point->ggaLatitude = String("");
  point->ggaLongitude = String("");
  point->accuracy = String("");
  point->altitude = String("");
  
  // start receiving GPS data
  mySerial->println("AT+CGNSTST=1");

  int i = 0;
  int numCommas = 0;
  unsigned long lastTime = millis();
  while (!found && (millis() - lastTime) < DATA_TIMEOUT) {
    
    // barrier to assure serial data available
    while (!mySerial->available() && (millis() - lastTime) < DATA_TIMEOUT);
    
    char c = mySerial->read();
    Serial.print(c);
    
    if (i < 5) {
      // Phrase GPGGA still not found
      
      if (c == GPGGA[i]) {
        i++;
      } else {
        i = 0;
      }
    } else {
      // Phrase GPGGA was found, now storing data
      Serial.println(F("Phrase found"));
      
      if (c == ',') {
        numCommas++;
      } else if (c == '*') {
        // Start receiving checksum
        int j = 0;
        String checksum = "";
        while (j < 2 && (millis() - lastTime) < DATA_TIMEOUT) {
          // barrier to assure serial data available
          while (!mySerial->available() && (millis() - lastTime) < DATA_TIMEOUT);         
          checksum += (char) mySerial->read();
          j++;
        }
        
        String expectedChecksum = gpsChecksumNMEA0183(string);
        Serial.println("");
        Serial.println(string);
        Serial.println(expectedChecksum);
        Serial.println(checksum);        
        
        if (checksum.equals(expectedChecksum)) {
          if (point->ggaLatitude.length() > 0 && point->ggaLongitude.length() > 0) {
            // Checksum is OK and latitude and longitude have valid data
            found = true;
          }
        } 
        
        if (!found) {
          // Either checksum is wrong or latitude and longitude are invalid
          // Reset search algorithm
          i = 0;
          point->ggaLatitude = String("");
          point->ggaLongitude = String("");
          point->accuracy = String("");
          point->altitude = String("");
          string = String(GPGGA);
          numCommas = 0;
          
          // jump into next iteration
          continue;
        }
        
      } else {
        if (numCommas == 2 || numCommas == 3) {
          // Store latitude
          point->ggaLatitude += (char) c;
        } else if (numCommas == 4 || numCommas == 5) {
          // Store longitude
          point->ggaLongitude += (char) c;
        } else if (numCommas == 8) {
          // Store accuracy
          point->accuracy += (char) c;
        } else if (numCommas == 9) {
          // Store altitude
          point->altitude += (char) c;
        }
      }
      
      string += (char) c;
    }
    
  }

  // stop receiving GPS data
  mySerial->println("AT+CGNSTST=0");  
}


// Check everything is ok
//AT <-- AT\nOK

// Send pin code
//AT+CPIN=2255 <-- OK\n\n+CPIN: READY\n\nSMS Ready

// Check if the MS is connected to the GPRS network
//AT+CGATT? <-- +CGATT: 1\n\nOK

// Set Bearer parameters for GPRS
//AT+SAPBR=3,1,"CONTYPE","GPRS" <-- OK

// Set Bearer parameters for APN
//AT+SAPBR=3,1,"APN","internet" <-- OK

// Open Bearer
//AT+SAPBR=1,1 <-- OK

// Init HTTP request
//AT+HTTPINIT <-- OK

// The second parameter is the URL
//AT+HTTPPARA="URL","www.google.com.hk" <-- OK

// Submit the request
//AT+HTTPACTION=0 <-- OK\n\n+HTTPACTION: 0,200,19606

// Read data from website
//AT+HTTPREAD

// Terminate http request
//AT+HTTPTERM


// Custom request:
//AT+CPIN=2255
//AT+CGATT?
//AT+SAPBR=3,1,"CONTYPE","GPRS"
//AT+SAPBR=3,1,"APN","internet"
//AT+SAPBR=1,1
//AT+CSTT="internet"
//AT+CIICR
//AT+CIFSR
//AT+CIPSPRT=0
//AT+CIPSTART="tcp","php.ayoza.com","80"
//AT+CIPSEND
// data
//AT+CIPCLOSE



