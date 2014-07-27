Maxim DS1337 Library for Arduino
================================

The Maxim DS1337 is an I<sup>2</sup>C RTC which operates at voltages between 1.8V and 5.5V, and can keep time at voltages down to 1.3V. In addition to keeping time, it is capable of outputting a square wave in a number of frequencies. It further has two programmable alarms, each of which can trigger either separate or common output pins, which can be connected to interrupt pins to wake a sleeping microcontroller. 

When powered by a 3.3V source, in time-keeping mode (that is, when the I<sup>2</sup>C interface is not being used, the operating current is around 400nA (with the square wave output disabled). At lower supply voltages, the operating current is even lower. 

The slave address is ``0x68``.

Library
-------

See [this documentation][arduino_libs] for details on installing Arduino libraries.

The two example programs provided with the library demonstrate setting the time and reading the time.

The SDA pin on the DS1337 should be connected to the SDA pin on the Arduino, and the SCL pin on the DS1337 should be connected to the SCL pin on the Arduino. 

Usage
-----

When the microcontroller first communicates with the DS1337, it should ensure the data integrity of the current time. This is done by checking the status register, which contains a bit which indicates whether the oscillator has stopped. If it has, the time on the DS1337 should be assumed to be invalid and must be reset. The following can be used:

	uint8_t status;
	ds1337_get_status(&status);
	
	if (status & DS1337_OSC_STOP_FLAG) {
		// Oscillator has stopped; time is invalid
	}
	
Time is read and written from the DS1337 in the form of a ``struct time_t`` object. 

	struct time_t {
	  uint8_t seconds;
	  uint8_t minutes;
	  uint8_t hours;
	  uint8_t day;
	  uint8_t month;
	  uint8_t year;
	};

The ``day`` is the day of the month. For the year, the millenium is assumed to be 2000, and the ``year`` variable represents the three least-signifiant digits (a value between 0 for the year 2000 and 199 for the year 2199). 

Alarms
------

There are two alarms. 

Alarm 1 has a minimum interval of one second, and a maximum interval of 31 days, 23 hours, 59 minutes, and 59 seconds. 

To enable alarm 1, first set it using one of the ``ds1337_set_alarm_1`` functions. Then clear the status flag using ``ds1337_clear_status(DS1337_A1_FLAG)``. Then, enable the alarm using ``ds1337_set_control(DS1337_A1_INT)``. Pin !INTA will go low when the alarm occurs, and will remain low until the alarm 1 flag is cleared. 

Alarm 2 has a minimum interval of one minute, and a maximum interval of 31 days, 23 hours, and 59 minutes. 

To enable alarm 2, set it using one of the ``ds1337_set_alarm_2`` functions. Then clear the status flag using ``ds1337_clear_status(DS1337_A2_FLAG)``. Then, enable the alarm using ``ds1337_set_control(DS1337_A2_INT)``. By default, pin !INTB will go low when the alarm occurs. However, if !INTB is configured to output a square wave, !INTA will go low instead. 

Square Wave Output
------------------

The DS1337 is capable of outputting a square wave from the SQW/!INTB at a selectable frequency. If this functionality is enabled (it's disabled by default), then alarm 2 will trigger !INTA instead. 

The following table contains ~INTB configurations and corresponding control register mask values.

| Frequency  | Period     | Control Register Mask  |
| :--------: | :--------: | :--------------------: |
| *Disabled* | *Disabled* | ``DS1337_SQW_A2``      |
| 1Hz        | 1s         | ``DS1337_SQW_1HZ``     |
| 4.096kHz   | 244&mu;s   | ``DS1337_SQW_4096HZ``  |
| 8.192kHz   | 122&mu;s   | ``DS1337_SQW_8192HZ``  |
| 32.768kHz  | 30.5&mu;s  | ``DS1337_SQW_32768HZ`` |

To configure the square wave output, clear the associated control register bits using ``ds1337_clear_control(DS1337_SQW_MASK)`` and then set the associated bits using ``ds1337_set_control`` with the desired mask from the table.

[arduino_libs]: http://arduino.cc/en/Guide/Libraries "Arduino documentation on installing libraries."