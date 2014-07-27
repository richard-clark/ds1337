#ifndef DS1337_H
#define DS1337_H

#include <inttypes.h>
#include "Wire.h"

extern TwoWire Wire;

// #define INCLUDE_UNIT_TESTS

// For verbose output during units tests, define these in the
// main sketch.
// For example:
// 		#define p(...) Serial.print(__VA_ARGS__)
//		#define pln(...) Serial.println(__VA_ARGS__)

#ifndef p
#define p(...)
#endif

#ifndef pln
#define pln(...)
#endif


// Device address
#define DS1337_ADDR  					0x68

// Registers
#define DS1337_REG_SECONDS              0x00
#define DS1337_REG_MINUTES              0x01
#define DS1337_REG_HOURS                0x02
#define DS1337_REG_DAY                  0x03
#define DS1337_REG_DATE                 0x04
#define DS1337_REG_MONTH                0x05
#define DS1337_REG_YEAR                 0x06
#define DS1337_REG_A1_SECONDS           0x07
#define DS1337_REG_A1_MINUTES           0x08
#define DS1337_REG_A1_HOUR              0x09
#define DS1337_REG_A1_DAY_DATE          0x0A
#define DS1337_REG_A2_MINUTES           0x0B
#define DS1337_REG_A2_HOUR              0x0C
#define DS1337_REG_A2_DAY_DATE          0x0D
#define DS1337_REG_CONTROL              0x0E
#define DS1337_REG_STATUS               0x0F

/**
If set, in an hour register (DS1337_REG_HOURS, DS1337_REG_A1_HOUR,
DS1337_REG_A2_HOUR, the hour is between 0 and 12, and the
(!AM)/PM bit indicates AM or PM.

If not set, the hour is between 0 and 23.
*/
#define DS1337_HOUR_12   			(0x01 << 6)

/**
If DS1337_HOUR_12 is set:
- If set, indicates PM
- If not set, indicates AM
*/
#define DS1337_PM_MASK        			(0x01 << 5)

/**
For DS1337_A1_DAY_DATE and DS1337_A2_DAY_DATE,
- If set, DAY_DATE register specifies a date (1-31).
- If not set, DAY_DATE register specifies a day (1-7).
*/
#define DS1337_DAY_MASK      			(0x01 << 6)

// If set, the oscillator has stopped since the last time
// this bit was cleared. 
#define DS1337_OSC_STOP_FLAG  			(0x01 << 7)

// If set, the A1 alarm has occurred. 
#define DS1337_A1_FLAG        			(0x01)

// If set, the A2 alarm has occurred. 
#define DS1337_A2_FLAG        			(0x01 << 1)

// Set to disable the oscillator
#define DS1337_OSC_DISABLE     			(0x01 << 7)

/**
These options control the behavior of the SQW/(!INTB) pin. 
*/
#define DS1337_SQW_MASK					(0x03 << 2)
#define DS1337_SQW_A2         			(0x01 << 2)
#define DS1337_SQW_32768HZ    			(0x03 << 2)
#define DS1337_SQW_8192HZ     			(0x02 << 2)
#define DS1337_SQW_4096HZ     			(0x01 << 2)
#define DS1337_SQW_1HZ        			(0x00 << 2)

// Alarm 1 interrupt enable.
#define DS1337_A1_INT         			0x01

// Alarm 2 interrupt enable. 
#define DS1337_A2_INT         			(0x01 << 1)

// Occurs when the number of I2C bytes available is less than the number requested. 
#define READ_ERROR  					5

#define decode_bcd(x) 					((x >> 4) * 10 + (x & 0x0F))
#define encode_bcd(x) 					((((x / 10) & 0x0F) << 4) + (x % 10))

#ifdef INCLUDE_UNIT_TESTS

uint8_t bcd_unit_testing();

#endif

/** Represents a time and date. */
struct time_t {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
};

void make_time(struct time_t* time, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
void format_time_str(struct time_t* time, char* buf);

#ifdef INCLUDE_UNIT_TESTING

int sub_str_to_int(char* str, char* buf, int start, int len);
uint8_t time_str_unit_testing();

#endif

uint8_t i2c_read(uint8_t addr, uint8_t* buf, uint8_t num);
uint8_t i2c_write(uint8_t addr, uint8_t* buf, uint8_t num);
uint8_t i2c_write_1(uint8_t addr, uint8_t b);
uint8_t ds1337_read_time(struct time_t* time);
uint8_t ds1337_write_time(struct time_t* time);
uint8_t ds1337_set_alarm_1_every_second();
uint8_t ds1337_set_alarm_1_at_second(uint8_t second);
uint8_t ds1337_set_alarm_1_at_minute(uint8_t minute, uint8_t second);
uint8_t ds1337_set_alarm_1_at_hour(uint8_t hour, uint8_t minute, uint8_t second);
uint8_t ds1337_set_alarm_1_at_day(uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
uint8_t ds1337_set_alarm_1_at_date(uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
uint8_t ds1337_set_alarm_2_every_minute() ;
uint8_t ds1337_set_alarm_2_at_minute(uint8_t minute);
uint8_t ds1337_set_alarm_2_at_hour(uint8_t hour, uint8_t minute);
uint8_t ds1337_set_alarm_2_at_day(uint8_t day, uint8_t hour, uint8_t minute);
uint8_t ds1337_set_alarm_2_at_date(uint8_t date, uint8_t hour, uint8_t minute);
uint8_t ds1337_get_control(uint8_t* ctrl);
uint8_t ds1337_set_control(uint8_t ctrl);
uint8_t ds1337_set_control_bits(uint8_t mask);
uint8_t ds1337_clear_control_bits(uint8_t mask);
uint8_t ds1337_get_status(uint8_t* stat);
uint8_t ds1337_set_status(uint8_t stat);
uint8_t ds1337_clear_status();
uint8_t ds1337_clear_status_bits(uint8_t mask);

#endif