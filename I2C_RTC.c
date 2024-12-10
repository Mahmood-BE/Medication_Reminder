#include <xc.h>
#include "I2C.h"
#include "I2C_RTC.h"
#define _XTAL_FREQ 8000000

#define RTC_ADDRESS		0b01101000

void i2c_rtc_initialize(void)
{
	unsigned int i;	
	
	// The current value of the DS1307 RTC register.
	unsigned char uc_current_register_value;
		
	// Read back the current value of register "Seconds".
	uc_current_register_value = uc_i2c_read(RTC_ADDRESS, 0x00);
	
	// If the CH bit is set, clear it to enable the oscillator.
	if ((uc_current_register_value & 0x80) != 0) 
	{
		uc_i2c_write(RTC_ADDRESS, 0x00, uc_current_register_value & 0x7F);
				
		// We need delay for the oscillator to start up.
		for(i = 0; i < 20; i ++)
		{
		__delay_ms(10);		
		}
	}
	
	
	// Read back the current value of register "Hours".
	uc_current_register_value = uc_i2c_read(RTC_ADDRESS, 0x02);
	
	// If it's in 12-hour mode, change it to 24-hour mode.
	if ((uc_current_register_value & 0x40) != 0) {
		uc_i2c_write(RTC_ADDRESS, 0x02, uc_current_register_value & 0xbf);
	}	
}	

unsigned char uc_i2c_rtc_get_seconds(void)
{
	unsigned char uc_i2c_data;
	
	// Read the value of register "Seconds".
	uc_i2c_data = uc_i2c_read(RTC_ADDRESS, 0x00);
	
	// Convert the BCD to binary and return the value.
	return uc_i2c_data;
}	

void i2c_rtc_set_seconds(unsigned char uc_value)
{
	// The current value of the DS1307 RTC register.
	unsigned char uc_current_register_value;
			
	// Make sure the value is < 60 seconds.
	if (uc_value < 60) {
		
		// Change the value into BCD.
		uc_value = ((uc_value / 10) << 4) + (uc_value % 10);
		
		// Read back the current value of register "Seconds".
		uc_current_register_value = uc_i2c_read(RTC_ADDRESS, 0x00);
		
		// We only interested in the CH bit, mask out the others.
		uc_current_register_value &= 0x80;
		
		// Write to the I2C RTC.
		uc_i2c_write(RTC_ADDRESS, 0x00, uc_current_register_value | uc_value);
	}	
}	

unsigned char uc_i2c_rtc_get_minutes(void)
{
	unsigned char uc_i2c_data;
	
	// Read the value of register "Minutes".
	uc_i2c_data = uc_i2c_read(RTC_ADDRESS, 0x01);
	
	// Convert the BCD to binary and return the value.
	return uc_i2c_data;
}	

void i2c_rtc_set_minutes(unsigned char uc_value)
{
	// Make sure the value is < 60 minutes.
	if (uc_value < 60) {
		
		// Change the value into BCD.
		uc_value = ((uc_value / 10) << 4) + (uc_value % 10);
		
		// Write to the I2C RTC.
		uc_i2c_write(RTC_ADDRESS, 0x01, uc_value);
	}	
}	

unsigned char uc_i2c_rtc_get_hours(void)
{
	unsigned char uc_i2c_data;
	
	// Read the value of register "Hours".
	uc_i2c_data = uc_i2c_read(RTC_ADDRESS, 0x02);
	
	// Convert the BCD to binary and return the value.
	return uc_i2c_data;
}	

void i2c_rtc_set_hours(unsigned char uc_value)
{
	// The current value of the DS1307 RTC register.
	unsigned char uc_current_register_value;	
	
	// Make sure the value is < 24 hours.
	if (uc_value < 24) {
		
		// Change the value into BCD.
		uc_value = ((uc_value / 10) << 4) + (uc_value % 10);
		
		// Read back the current value of register "Hours".
		uc_current_register_value = uc_i2c_read(RTC_ADDRESS, 0x02);
		
		// We only interested in the 12-hour/24-hour mode bit, mask out the others.
		uc_current_register_value &= 0x40;
		
		// Write to the I2C RTC.
		uc_i2c_write(RTC_ADDRESS, 0x02, uc_current_register_value | uc_value);
	}	
}	
