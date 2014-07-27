#include <Wire.h>
#include <DS1337.h>

/**

Maxim DS1337 RTC Time Reading Example 
Richard Clark, 2014

Read the time stored by a DS1337 real-time clock every second, 
and print it over the serial interface.

*/

// A structure to hold the time read from the device
struct time_t time;

// A character array to hold the string representation of the time
char time_str[20];

void setup() {
  
  // Initialize the I2C bus
  Wire.begin();
  
  // Initialize the serial interface
  Serial.begin(115200);
  while(!Serial);
  
  // Once serial has been inialized, print a welcome message
  Serial.println("Maxim DS1337 RTC Example"); 
}

void loop() {
  
  // Attempt to read the time. 
  if (ds1337_read_time(&time)) {
    Serial.println("Unable to read time--an I2C error has occured.");
  } else {
    // The time has been read successfully--print it.
    format_time_str(&time, time_str);
    Serial.println(time_str);
  }
 
  delay(1000);
  
}
