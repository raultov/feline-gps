
#ifndef _GPS_
#define _GPS_1

// GPGGA,180341.000,4025.7288,N,00338.5588,W,1,5,2.90,676.1,M,51.7,M,,*44

typedef struct Point {
  String ggaLatitude = "";
  String ggaLongitude = "";
  String accuracy = "";
  String altitude = "";
};

/**
  Power on the GPS module

  mySerial must have been initialized by calling method begin(baudrate)  
*/
void gpsPowerOn(SoftwareSerial * mySerial);

/**
  Power off the GPS module

  mySerial must have been initialized by calling method begin(baudrate)  
*/
void gpsPowerOff(SoftwareSerial * mySerial);

/**
  Return a Point
  
  This method must be called after calling gpsPowerOn
  mySerial must have been initialized by calling method begin(baudrate)  
*/
Point * gpsGetPoint(SoftwareSerial * mySerial);

#endif 
