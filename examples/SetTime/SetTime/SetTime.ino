#include <Wire.h>
#include <DS1337.h>

/**

Maxim DS1337 RTC Time Setting Example 
Richard Clark, 2014

Set the time stored by a DS1337 real-time clock.

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

  // Set the date of the time structure to June 27, 2014 and 
  // the time to 18:45:00 (6:45:00 pm)
  make_time(&time, 014, 6, 27, 18, 45, 00);

  // Set the clock
  if (ds1337_write_time(&time)) {
    Serial.println("Unable to set the time--an I2C error has occured.");
  } else {
    Serial.println("Clock set successfully!");
  }

}

void loop() {
  
  // Nothing to do here.
  
}
