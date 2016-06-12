

void gpsInitPower() {
  
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



