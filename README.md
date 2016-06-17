# feline-gps
This is an Arduino Leonardo project that it is meant to upload tracking data to an API web.
It uses a shield SIM808 which consits of a GPRS module and a builtin GPS.

Note:
I had to increase buffer size in SoftwareSerial library:
#define _SS_MAX_RX_BUFF 128 // RX buffer size
The reason is that the default value 64 was not enough to receive data from SIM808

