#include <Arduino.h>
#include <SoftwareSerial.h>
#include "gps.h"
#include "network.h"
#include "api_conf.h"

#define DATA_TIMEOUT 10000L
// Request constants
#define CONTENT_TYPE_API "\"application/x-www-form-urlencoded\""
// Response constants
#define ACCESS_TOKEN "access_token\":\""
#define HTTP_STATUS_OK "200"

void showSerialData(SoftwareSerial * mySerial) {
  while(mySerial->available() != 0)
    Serial.write(mySerial->read());
}

void initializeConnection(SoftwareSerial * mySerial) {
  mySerial->println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); //setting the SAPBR, the connection type is using gprs
  delay(1000);
  showSerialData(mySerial);

  String apn = String(APN);
  mySerial->println("AT+SAPBR=3,1,\"APN\"," + apn); //setting the APN, the second need you fill in your local apn server
  delay(4000);
  showSerialData(mySerial);
  
  mySerial->println("AT+SAPBR=1,1"); //setting the SAPBR, for detail you can refer to the AT command mamual
  delay(2000);
  showSerialData(mySerial);
    
  mySerial->println("AT+CGATT?");
  delay(100);
  showSerialData(mySerial);  
  
  mySerial->println("AT+CSTT=" + apn); //start task and setting the APN,
  delay(1000);
  showSerialData(mySerial);

  mySerial->println("AT+CIICR"); //bring up wireless connection
  delay(300);
  showSerialData(mySerial);  
  
  mySerial->println("AT+CIFSR"); //get local IP adress
  delay(2000);
  showSerialData(mySerial);

  mySerial->println("AT+CIPSPRT=0");
  delay(3000);
  showSerialData(mySerial);  
}

void networkSetUserToken(SoftwareSerial * mySerial) {
  userToken = "";
  String buf = String("");
  
  mySerial->println("AT+HTTPINIT");
  delay(500);
  showSerialData(mySerial);
  
  mySerial->println("AT+HTTPPARA=\"CID\",1");
  delay(500);
  showSerialData(mySerial);
  
  buf = String(URL_TOKEN_API);
  mySerial->println("AT+HTTPPARA=\"URL\"," + buf);
  delay(500);
  showSerialData(mySerial);  
  
  mySerial->println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(500);
  showSerialData(mySerial);  
  
  buf = String(HEADER_TOKEN_API1);
  mySerial->print("AT+HTTPPARA=\"USERDATA\"," + buf);
  delay(500);
  showSerialData(mySerial);
  buf = String(HEADER_TOKEN_API2);
  mySerial->println(buf);
  delay(500);
  showSerialData(mySerial);   
  
  mySerial->println("AT+HTTPDATA=75,20000");
  delay(500);
  showSerialData(mySerial);      

  mySerial->print(BODY1);
  delay(500);
  showSerialData(mySerial);
  mySerial->println(BODY2);
  delay(500);
  showSerialData(mySerial);  
  
  mySerial->println("AT+HTTPACTION=1");
  delay(5000);
  showSerialData(mySerial);
  
  mySerial->println("AT+HTTPREAD");
  delay(1000);

  bool found = false;
  int i = 0;
  unsigned long lastTime = millis();
  while (!found && (millis() - lastTime) < DATA_TIMEOUT) {

    if (mySerial->available()) {
      char c = mySerial->read();
      Serial.print(c);
      
      if (i < 15) {
        // Phrase GPGGA still not found
        
        if (c == ACCESS_TOKEN[i]) {
          i++;
        } else {
          i = 0;
        }
      } else {
        // Phrase access_token was found, now storing data
        
        if (c != '\"') {
          userToken += c; 
        } else {
          found = true;
        }
      }
    } 
  }
  
  mySerial->println("AT+HTTPTERM");
  delay(500);
  showSerialData(mySerial);
}

bool uploadPoint(Point * point, SoftwareSerial * mySerial) {
  String buf = String("");
  
  mySerial->println("AT+HTTPINIT");
  delay(500);
  showSerialData(mySerial);
  
  mySerial->println("AT+HTTPPARA=\"CID\",1");
  delay(500);
  showSerialData(mySerial);
  
  buf = String(URL_UPLOAD_POINT_API);
  mySerial->println("AT+HTTPPARA=\"URL\"," + buf);
  delay(500);
  showSerialData(mySerial);  
  
  mySerial->println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(500);
  showSerialData(mySerial); 
 
  // Here we use the user token stored in the global variable userToken
  mySerial->print("AT+HTTPPARA=\"USERDATA\", \"Authorization: bearer ");
  delay(500);
  showSerialData(mySerial);
  mySerial->println(userToken + "\"");
  delay(500);
  showSerialData(mySerial); 
  
  buf = String("latitude=" + point->ggaLatitude + "&longitude=" + point->ggaLongitude + "&accuracy=" + point->accuracy + "&altitude=" + point->altitude);
  mySerial->println("AT+HTTPDATA=" + String(buf.length()) + ",20000");
  delay(500);
  showSerialData(mySerial);    
 
  for (int i = 0; i < buf.length(); i++) {
    mySerial->print(buf.charAt(i));
    delay(10);
  }
  
  mySerial->println("AT+HTTPACTION=1");
  delay(5000);

  bool found = false;
  int i = 0;
  unsigned long lastTime = millis();
  while (!found && (millis() - lastTime) < DATA_TIMEOUT) {
    
    if (mySerial->available()) {
      char c = mySerial->read();
      Serial.print(c);
      
      if (i < 3) {
        // Phrase 200 still not found
        
        if (c == HTTP_STATUS_OK[i]) {
          i++;
        } else {
          i = 0;
        }
      } else {
        // Phrase 200 was found, now storing data
        found = true;
      }  
    }  
  }
  
  mySerial->println("AT+HTTPTERM");
  delay(500);
  showSerialData(mySerial);  
  
  return found;
}
/*
void networkSetUserToken(SoftwareSerial * mySerial) {
  userToken = "";
  
  String protocol = String(PROTOCOL_API);
  String server = String(SERVER_API);
  String port = String(PORT_API);
  mySerial->println("AT+CIPSTART=" + protocol + "," + server + "," + port); //start up the connection
  delay(2000);
  showSerialData(mySerial);  
  
  mySerial->println("AT+CIPSEND"); //begin send data to remote server
  delay(2000);
  showSerialData(mySerial);
  
  mySerial->println("POST /feline-rest/oauth/token HTTP/1.1");
  delay(500);
  showSerialData(mySerial);
  mySerial->println("Host: ayoza.com:8081");
  delay(500);
  showSerialData(mySerial);  
  //mySerial->println("Connection: keep-alive");
  //delay(500);
  //showSerialData(mySerial);  
  mySerial->println("Content-Length: 73");
  delay(500);  
  showSerialData(mySerial);  
  mySerial->println("Content-Type: application/x-www-form-urlencoded");
  delay(500);
  showSerialData(mySerial);  
  mySerial->print("Authorization: Basic Y2xpZW50LXdpdGgtcmVmcmVzaC10b2tl");
  delay(500);
  showSerialData(mySerial);
  mySerial->println("bjpjbGllbnQtd2l0aC1yZWZyZXNoLXRva2VuLXNlY3JldA==");
  delay(500);
  showSerialData(mySerial);  
  //mySerial->println("Accept: *\  String port = String(PORT_API);
  mySerial->println("AT+CIPSTART=" + protocol + "," + server + "," + port); //start up the connection
  delay(2000);
  showSerialData(mySerial);  
  
  mySerial->println("AT+CIPSEND"); //begin send data to remote server
  delay(2000);
  showSerialData(mySerial);
  
  mySerial->println("POST /feline-rest/oauth/token HTTP/1.1");
  delay(500);
  showSerialData(mySerial);
  mySerial->println("Host: ayoza.com:8081");
  delay(500);
  showSerialData(mySerial);  
  //mySerial->println("Connection: keep-alive");
  //delay(500);
  //showSerialData(mySerial);  
  mySerial->println("Content-Length: 73");
  delay(500);  
  showSerialData(mySerial);  
  mySerial->println("Content-Type: application/x-www-form-urlencoded");
  delay(500);
  showSerialData(mySerial);  
  mySerial->print("Authorization: Basic Y2xpZW50LXdpdGgtcmVmcmVzaC10b2tl");
  delay(500);
  showSerialData(mySerial);
  mySerial->println("bjpjbGllbnQtd2l0aC1yZWZyZXNoLXRva2VuLXNlY3JldA==");
  delay(500);
  showSerialData(mySerial);  
  //mySerial->println("Accept: *\/*");
  //delay(500);
  //showSerialData(mySerial);  
  //mySerial->println("Accept-Encoding: gzip, deflate");
  //delay(500);
  //showSerialData(mySerial);   
  
  mySerial->println("");
  delay(500);
  showSerialData(mySerial);
  mySerial->print("grant_type=password&username=%2B34644164356");
  delay(500);
  showSerialData(mySerial);
  mySerial->print("&password=123456&scope=general");
  delay(500);
  showSerialData(mySerial);
  
  mySerial->println((char)26);//sending
  delay(500); //waiting for reply, important! the time is based on the condition of internet 
  mySerial->println();
  
  unsigned long last = millis();
  
  while (millis() < (last + 10000L)) {
    while(mySerial->available() != 0)
      Serial.write(mySerial->read());
  }
  
  mySerial->println("AT+CIPCLOSE"); //close the connection
  
  last = millis();

  while (millis() < (last + 10000L)) {
    while(mySerial->available() != 0)
      Serial.write(mySerial->read());
  }
}/*");
  //delay(500);
  //showSerialData(mySerial);  
  //mySerial->println("Accept-Encoding: gzip, deflate");
  //delay(500);
  //showSerialData(mySerial);   
  
  mySerial->println("");
  delay(500);
  showSerialData(mySerial);
  mySerial->print("grant_type=password&username=%2B34644164356");
  delay(500);
  showSerialData(mySerial);
  mySerial->print("&password=123456&scope=general");
  delay(500);
  showSerialData(mySerial);
  
  mySerial->println((char)26);//sending
  delay(500); //waiting for reply, important! the time is based on the condition of internet 
  mySerial->println();
  
  unsigned long last = millis();
  
  while (millis() < (last + 10000L)) {
    while(mySerial->available() != 0)
      Serial.write(mySerial->read());
  }
  
  mySerial->println("AT+CIPCLOSE"); //close the connection
  
  last = millis();

  while (millis() < (last + 10000L)) {
    while(mySerial->available() != 0)
      Serial.write(mySerial->read());
  }
}
*/

void networkSetPinCode(String pinCode, SoftwareSerial * mySerial) {
  mySerial->println("AT+CPIN=" + pinCode);
}


















