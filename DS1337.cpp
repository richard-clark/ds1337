#include "DS1337.h"

extern "C" {
  #include <inttypes.h>
}


/**
 * \defgroup ds1337_time_t Time Structure and Associated Methods
 */

/** 
 * \brief Set the values in a struct time_t instance. 
 * 
 * \param time A pointer to a struct time_t instance. 
 * \param year The year.
 * \param month The month. 
 * \param day The day. 
 * \param hour The hour. 
 * \param minute The minute. 
 * \param second The second. 
 */
void make_time(struct time_t* time, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
  time->year = year;
  time->month = month;
  time->day = day;
  time->hour = hour;
  time->minute = minute;
  time->second = second;
}

/**
 * \brief Output a string representation of a struct time_t object.
 *
 * Requires a buffer size of at least 20 characters. 
 *
 * \param time A pointer to a struct time_t instance. 
 * \param buf A pointer to a character buffer in which to store the string.
 */
void format_time_str(struct time_t* time, char* buf) {
	// Year
	buf[0] = '2';
	if (time->year < 10) {
		buf[1] = '0';
		buf[2] = '0';
		buf[3] = '0' + time->year;
	} else if (time->year < 100) {
		buf[1] = '0';
		buf[2] = '0' + (time->year / 10);
		buf[3] = '0' + (time->year % 10);
	} else {
		buf[1] = '1';
		buf[2] = '0' + ((time->year - 100) / 10);
		buf[3] = '0' + ((time->year - 100) % 10);
	}
	buf[4] = '-';
	
	// Month
	if (time->month < 10) {
		buf[5] = '0';
		buf[6] = '0' + time->month;
	} else {
		buf[5] = '0' + (time->month / 10);
		buf[6] = '0' + (time->month % 10);
	}
	buf[7] = '-';
	
	// Day
	if (time->day < 10) {
		buf[8] = '0';
		buf[9] = '0' + time->day;
	} else {
		buf[8] = '0' + (time->day / 10);
		buf[9] = '0' + (time->day % 10);
	}
	buf[10] = 'T';

	// Hour
	if (time->hour < 10) {
		buf[11] = '0';
		buf[12] = '0' + time->hour;
	} else {
		buf[11] = '0' + (time->hour / 10);
		buf[12] = '0' + (time->hour % 10);
	}
	buf[13] = ':';
	
	// Minute
	if (time->minute < 10) {
		buf[14] = '0';
		buf[15] = '0' + time->minute;
	} else {
		buf[14] = '0' + (time->minute / 10);
		buf[15] = '0' + (time->minute % 10);
	}
	buf[16] = ':';
	
	// Second
	if (time->second < 10) {
		buf[17] = '0';
		buf[18] = '0' + time->second;
	} else {
		buf[17] = '0' + (time->second / 10);
		buf[18] = '0' + (time->second % 10);
	}
	
	buf[19] = 0;
}

/**
 * \defgroup ds1337_i2c I2C Functions
 */

/**
 * \brief Read data from an I2C device. 
 *
 * \param addr The address of the device from which to read. 
 * \param buf A pointer to a buffer in which to store the data. 
 * \param num The number of bytes to read. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t i2c_read(uint8_t addr, uint8_t* buf, uint8_t num) {
  Wire.requestFrom(addr, num);
  
  if (Wire.available() < num) {
    return READ_ERROR;
  }
  
  for (uint8_t i = 0; i < num; i++) {
    buf[i] = Wire.read();
  }
  
  return 0;
}

/**
 * \brief Write data to an I2C device. 
 *
 * \param addr The address of the device to which to write. 
 * \param buf A pointer to a buffer from which to read the data. 
 * \param num The number of bytes to write. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t i2c_write(uint8_t addr, uint8_t* buf, uint8_t num) {
  Wire.beginTransmission(addr);
  for (uint8_t i = 0; i < num; i++) {
    Wire.write(buf[i]);
  }
  return Wire.endTransmission();
}

/**
 * \brief Write a single byte to an I2C device. 
 *
 * \param addr The address of the device to which to write. 
 * \param b The byte to write. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t i2c_write_1(uint8_t addr, uint8_t b) {
  Wire.beginTransmission(addr);
  Wire.write(b);
  return Wire.endTransmission();
}

/**
 * \defgroup ds1337_time Time Getter and Setter Methods
 */

/** 
 * \brief Read the current time. 
 * 
 * \param time A pointer to a struct time_t instance in which to store the time. 
 * 
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_read_time(struct time_t* time) {
  uint8_t buf[7];
  
  uint8_t res = i2c_write_1(DS1337_ADDR, DS1337_REG_SECONDS);
  
  if (res) {
    p("I2C write error: ");
    pln(res, DEC);
    return 1;
  }
  
  res = i2c_read(DS1337_ADDR, buf, 7);
  
  if (res) {
    pln("I2C read error.");
    return 2;
  }
  
  time->second = decode_bcd(buf[0]);
  time->minute = decode_bcd(buf[1]);
  
  if (buf[2] & DS1337_HOUR_12) {
    time->hour = ((buf[2] >> 4) & 0x01) * 12 + ((buf[2] >> 5) & 0x01) * 12;
  } else {
    time->hour = decode_bcd(buf[2]);
  }
  
  time->day = decode_bcd(buf[4]);
  time->month = decode_bcd(buf[5] & 0x1F);
  time->year = 100 * ((buf[5] >> 7) & 0x01) + decode_bcd(buf[6]); 
  
  return 0;
  
}

/** 
 * \brief Set the time. 
 * 
 * \param time A pointer to a struct time_t instance containing the time to set. 
 * 
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_write_time(struct time_t* time) {
  uint8_t buf[8];
  
  buf[0] = DS1337_REG_SECONDS;
  buf[1] = encode_bcd(time->second);
  buf[2] = encode_bcd(time->minute);
  buf[3] = encode_bcd(time->hour); // Time always stored in 24-hour format
  buf[4] = 1; // Not used 
  buf[5] = encode_bcd(time->day);
  buf[6] = ((time->year / 100) << 7) | encode_bcd(time->month);
  buf[7] = encode_bcd((time->year) % 100);
  
  uint8_t res = i2c_write(DS1337_ADDR, buf, 8);
  
  if (res) {
    p("I2C write error: ");
    pln(res, DEC);
    return 1;
  }
  
  return 0;
  
}

/**
 * \defgroup ds1337_alarm_1 Alarm 1 Setter Methods
 */

/**
 * \brief Set alarm 1 to occur every second.
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_1_every_second() {
	uint8_t buf[5];
	buf[0] = DS1337_REG_A1_SECONDS;
	buf[1] = 0x80;
	buf[2] = 0x80;
	buf[3] = 0x80;
	buf[4] = 0x80;
	return i2c_write(DS1337_ADDR, buf, 5);
}

/**
 * \brief Set alarm 1 to occur at a specified second of every minute.
 *
 * \param second The second at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_1_at_second(uint8_t second) {
	uint8_t buf[5];
	buf[0] = DS1337_REG_A1_SECONDS;
	buf[1] = encode_bcd(second);
	buf[2] = 0x80;
	buf[3] = 0x80;
	buf[4] = 0x80;
	return i2c_write(DS1337_ADDR, buf, 5);
}

/**
 * \brief Set alarm 1 to occur at a specified minute and second of every hour.
 *
 * \param minute The minute at which the alarm should occur. 
 * \param second The second at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_1_at_minute(uint8_t minute, uint8_t second) {
	uint8_t buf[5];
	buf[0] = DS1337_REG_A1_SECONDS;
	buf[1] = encode_bcd(second);
	buf[2] = encode_bcd(minute);
	buf[3] = 0x80;
	buf[4] = 0x80;
	return i2c_write(DS1337_ADDR, buf, 5);
}

/**
 * \brief Set alarm 1 to occur at a specified hour, minute, and second of every day.
 *
 * \param hour The hour at which the alarm should occur. 
 * \param minute The minute at which the alarm should occur. 
 * \param second The second at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_1_at_hour(uint8_t hour, uint8_t minute, uint8_t second) {
	uint8_t buf[5];
	buf[0] = DS1337_REG_A1_SECONDS;
	buf[1] = encode_bcd(second);
	buf[2] = encode_bcd(minute);
	buf[3] = encode_bcd(hour);
	buf[4] = 0x80;
	return i2c_write(DS1337_ADDR, buf, 5);
}

/**
 * \brief Set alarm 1 to occur at a specified day, hour, minute, and second once per week.
 *
 * \param date The day of the week at which the alarm should occur (in the range 1-7).
 * \param hour The hour at which the alarm should occur. 
 * \param minute The minute at which the alarm should occur.
 * \param second The second at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_1_at_day(uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	uint8_t buf[5];
	buf[0] = DS1337_REG_A1_SECONDS;
	buf[1] = encode_bcd(second);
	buf[2] = encode_bcd(minute);
	buf[3] = encode_bcd(hour);
	buf[4] = DS1337_DAY_MASK | encode_bcd(day);
	return i2c_write(DS1337_ADDR, buf, 5);
}

/**
 * \brief Set alarm 1 to occur at a specified day, hour, minute, and second once per month.
 *
 * \param date The day at which the alarm should occur (in the range 1-31).
 * \param hour The hour at which the alarm should occur. 
 * \param minute The minute at which the alarm should occur.
 * \param second The second at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_1_at_date(uint8_t date, uint8_t hour, uint8_t minute, uint8_t second) {
	uint8_t buf[5];
	buf[0] = DS1337_REG_A1_SECONDS;
	buf[1] = encode_bcd(second);
	buf[2] = encode_bcd(minute);
	buf[3] = encode_bcd(hour);
	buf[4] = encode_bcd(date);
	return i2c_write(DS1337_ADDR, buf, 5);
}

/**
 * \defgroup ds1337_alarm_2 Alarm 2 Setter Methods
 */

/**
 * \brief Set alarm 2 to occur every minute (at 0 seconds). 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_2_every_minute() {
	uint8_t buf[4];
	buf[0] = DS1337_REG_A2_MINUTES;
	buf[1] = 0x80;
	buf[2] = 0x80;
	buf[3] = 0x80;
	return i2c_write(DS1337_ADDR, buf, 4);
}

/**
 * \brief Set alarm 2 to occur at a specified minute of every hour.
 *
 * (At 0 seconds). 
 *
 * \param minute The minute at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_2_at_minute(uint8_t minute) {
	uint8_t buf[4];
	buf[0] = DS1337_REG_A2_MINUTES;
	buf[1] = encode_bcd(minute);
	buf[2] = 0x80;
	buf[3] = 0x80;
	return i2c_write(DS1337_ADDR, buf, 4);
}

/**
 * \brief Set alarm 2 to occur at a specified hour and minute of every day.
 *
 * (At 0 seconds). 
 *
 * \param hour The hour at which the alarm should occur. 
 * \param minute The minute at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_2_at_hour(uint8_t hour, uint8_t minute) {
	uint8_t buf[4];
	buf[0] = DS1337_REG_A2_MINUTES;
	buf[1] = encode_bcd(minute);
	buf[2] = encode_bcd(hour);
	buf[3] = 0x80;
	return i2c_write(DS1337_ADDR, buf, 4);
}

/**
 * \brief Set alarm 2 to occur at a specified day, hour, and minute once per week.
 *
 * (At 0 seconds). 
 *
 * \param day The day at which the alarm should occur (in the range 1-7).
 * \param hour The hour at which the alarm should occur. 
 * \param minute The minute at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_2_at_day(uint8_t day, uint8_t hour, uint8_t minute) {
	uint8_t buf[4];
	buf[0] = DS1337_REG_A2_MINUTES;
	buf[1] = encode_bcd(minute);
	buf[2] = encode_bcd(hour);
	buf[3] = DS1337_DAY_MASK | encode_bcd(day);
	return i2c_write(DS1337_ADDR, buf, 4);
}

/**
 * \brief Set alarm 2 to occur at a specified day, hour, and minute once per month.
 *
 * (At 0 seconds). 
 *
 * \param date The day at which the alarm should occur (in the range 1-31).
 * \param hour The hour at which the alarm should occur. 
 * \param minute The minute at which the alarm should occur. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_alarm_2_at_date(uint8_t date, uint8_t hour, uint8_t minute) {
	uint8_t buf[4];
	buf[0] = DS1337_REG_A2_MINUTES;
	buf[1] = encode_bcd(minute);
	buf[2] = encode_bcd(hour);
	buf[3] = encode_bcd(date);
	return i2c_write(DS1337_ADDR, buf, 4);
}

/**
 * \defgroup ds1337_control Control Register Methods
 */

/**
 * \brief Get the value of the control register.
 *
 * \param ctrl A pointer to a value in which to store the value of the control register. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_get_control(uint8_t* ctrl) {
   uint8_t res = i2c_write_1(DS1337_ADDR, DS1337_REG_CONTROL);
   
   if (res) {
     return res;
   }
   
   res = i2c_read(DS1337_ADDR, ctrl, 1);
   
   if (res) {
     return res;
   }
   
   return 0;
}

/**
 * \brief Set the value of the control register.
 *
 * \param ctrl The value to set. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_control(uint8_t ctrl) {
   uint8_t buf[2];
   buf[0] = DS1337_REG_CONTROL;
   buf[1] = ctrl;
   return i2c_write(DS1337_ADDR, buf, 2);
}

/**
 * \brief Set the specified bits in the control register.
 *
 * \param mask A mask specifying which bits to set. (High bits will be set.)
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_control_bits(uint8_t mask) { // set bits
	uint8_t ctrl;
	uint8_t res = ds1337_get_control(&ctrl);
	if (res) { return res; }
	ctrl |= mask;
	return ds1337_set_control(ctrl);
}

/**
 * \brief Clear the specified bits in the control register.
 *
 * \param mask A mask specifying which bits to clear. (High bits will be cleared.) 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_clear_control_bits(uint8_t mask) {
	return ds1337_set_control(~mask);
}

/**
 * \defgroup ds1337_status Status Register Methods
 */
 
/**
 * \brief Get the value of the status register.
 *
 * \param ctrl A pointer to a value in which to store the value of the status register. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_get_status(uint8_t* stat) {
  uint8_t res = i2c_write_1(DS1337_ADDR, DS1337_REG_STATUS);
  
  if (res) {
    return res;
  }
  
  res = i2c_read(DS1337_ADDR, stat, 1);
  
  if (res) {
    return res;
  }
  
  return 0;
}

/**
 * \brief Set the value of the status register.
 *
 * \param stat The value to set. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_set_status(uint8_t stat) {
   uint8_t buf[2];
   buf[0] = DS1337_REG_STATUS;
   buf[1] = stat;
   return i2c_write(DS1337_ADDR, buf, 2);
}

/**
 * \brief Clear the status register. 
 * 
 * \return 0 on success; otherwise an I2C error. 
 */
uint8_t ds1337_clear_status() {
  uint8_t buf[2]; 
  buf[0] = DS1337_REG_STATUS;
  buf[1] = 0;
  return i2c_write(DS1337_ADDR, buf, 2);
}

/**
 * \brief Clear the specified bits in the status register.
 *
 * \param mask A mask specifying which bits to clear. (High bits will be cleared.) 
 *
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1337_clear_status_bits(uint8_t mask) { // set bits
	uint8_t stat;
	uint8_t res = ds1337_get_status(&stat);
	if (res) { return res; }
	stat |= (~mask);
	return ds1337_set_status(stat);
}
