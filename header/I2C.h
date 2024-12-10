void i2c_initialize(void);
unsigned char b_i2c_check_error_flag(void);
unsigned char uc_i2c_read(unsigned char uc_slave_address, unsigned char uc_register_add);
void uc_i2c_write(unsigned char uc_slave_address, unsigned char uc_register_add, unsigned char uc_data);
void i2c_idle(void);

unsigned char b_i2c_error_flag = 0;

void i2c_initialize(void)
{
	// Set the SCL and SDA pin as input.
	
	TRISCbits.TRISC3 = 1;
	TRISCbits.TRISC4 = 1;
	
	// Set the I2C clock frequency.
	SSPADD = 4;
	
	SSPSTATbits.SMP = 0;	//Slew Rate Control enable for 400KHz clock
	
	// Select I2C master mode, clock = FOSC / (4 * (SSPADD+1) ).
	SSPCON1bits.SSPM3 = 1;
	SSPCON1bits.SSPM2 = 0;
	SSPCON1bits.SSPM1 = 0;
	SSPCON1bits.SSPM0 = 0;						
	
	// Clear the Write Collision Detect bit.
	SSPCON1bits.WCOL = 0;
	
	// Clear the Receive Overflow Indicator bit.
	SSPCON1bits.SSPOV = 0;						
	
	// Enable the MSSP module.
	SSPCON1bits.SSPEN = 1;	
	
	// Send stop condition, stop any previous communication
	SSPCON2bits.PEN = 1;
	while (SSPCON2bits.PEN == 1);					
}	

unsigned char b_i2c_check_error_flag(void)
{
	return b_i2c_error_flag;	
}	

unsigned char uc_i2c_read(unsigned char uc_slave_address, unsigned char uc_register_add)
{
	// Buffer for the received byte.
	unsigned char uc_received_byte;
	unsigned long count = 10000;
	// Clear the error flag before we start a new I2C operation.
	b_i2c_error_flag = 0;
	
	// Send start bit.	
	SSPCON2bits.SEN = 1;
	while (SSPCON2bits.SEN == 1);
	
	// Send slave address and indicate to write.
	SSPBUF = (uc_slave_address << 1) & 0xfe;

	
	// Wait for slave to acknowledge.
	//while (SSPSTATbits.R_W == 1);	
	
	i2c_idle();	//wait for the message sending process complete
	
	// If slave does not acknowledge...
	if (SSPCON2bits.ACKSTAT == 1) {
		// Send stop bit.
		SSPCON2bits.PEN = 1;
		while (SSPCON2bits.PEN == 1);
		
		// Set the error flag and exit.
		b_i2c_error_flag = 1;
		return 0;
	}
	
	// Send the register address that we want to read and I2C will start transmitting out
	SSPBUF = uc_register_add;
	
	// Wait for slave to acknowledge.
	//while (SSPSTATbits.R_W == 1);
	
	i2c_idle();	//wait for the message sending process complete
	
	// If slave does not acknowledge...
	if (SSPCON2bits.ACKSTAT == 1) {
		// Send stop bit.
		SSPCON2bits.PEN = 1;
		while (SSPCON2bits.PEN == 1);
		
		// Set the error flag and exit.
		b_i2c_error_flag = 1;
		return 0;
	}	
	
	// Send repeated start bit.
	SSPCON2bits.RSEN = 1;
	while (SSPCON2bits.RSEN == 1);
	
	// Send slave address and indicate to read.
	SSPBUF = (uc_slave_address << 1) | 0x01;
	
	// Wait for slave to acknowledge.
	//while (SSPSTATbits.R_W == 1);
	
	i2c_idle();	//wait for the message sending process complete
	
	// If slave does not acknowledge...
	if (SSPCON2bits.ACKSTAT == 1) {
		// Send stop bit.
		SSPCON2bits.PEN = 1;
		while (SSPCON2bits.PEN == 1);
		
		// Set the error flag and exit.
		b_i2c_error_flag = 1;
		return 0;
	}
	
	// Enable receive.
	SSPCON2bits.RCEN = 1;
	
	// Wait until the data is received.
	while (SSPSTATbits.BF == 0) 
	{		
		// If timeout...
		if (--count == 0)
		{
			// Send stop bit.
			SSPCON2bits.PEN = 1;
			while (SSPCON2bits.PEN == 1);
			
			// Set the error flag and exit.
			b_i2c_error_flag = 1;
			return 0;
		}
	}
	
	// Read the received data.
	uc_received_byte = SSPBUF;
	
	// Send Not Acknowledge.
	SSPCON2bits.ACKDT = 1;
	SSPCON2bits.ACKEN = 1;
	while (SSPCON2bits.ACKEN == 1);
	
	// Send stop bit
	SSPCON2bits.PEN = 1;
	while (SSPCON2bits.PEN == 1);
	
	// Clear the error flag and return the received data.
	b_i2c_error_flag = 0;
	return uc_received_byte;
}

void uc_i2c_write(unsigned char uc_slave_address, unsigned char uc_register_add, unsigned char uc_data)
{
	// Clear the error flag before we start a new I2C operation.
	b_i2c_error_flag = 0;
	
	// Send start bit.	
	SSPCON2bits.SEN = 1;
	while (SSPCON2bits.SEN == 1);
	
	// Send slave address and indicate to write.
	SSPBUF = (uc_slave_address << 1) & 0xfe;
	
	// Wait for slave to acknowledge.
	//while (SSPSTATbits.R_W == 1);
	
	i2c_idle();	//wait for the message sending process complete
	
	// If slave does not acknowledge...
	if (SSPCON2bits.ACKSTAT == 1)
	{
		// Send stop bit.
		SSPCON2bits.PEN = 1;
		while (SSPCON2bits.PEN == 1);
		
		// Set the error flag and exit.
		b_i2c_error_flag = 1;
		return;
	}
	
	// Send the register address that we want to write.
	SSPBUF = uc_register_add;
	
	// Wait for slave to acknowledge.
	//while (SSPSTATbits.R_W == 1);
	
	i2c_idle();	//wait for the message sending process complete
	
	// If slave does not acknowledge...
	if (SSPCON2bits.ACKSTAT == 1) 
	{
		// Send stop bit.
		SSPCON2bits.PEN = 1;
		while (SSPCON2bits.PEN == 1);
		
		// Set the error flag and exit.
		b_i2c_error_flag = 1;
		return;
	}
	
	// Send the data.
	SSPBUF = uc_data;
	
	// Wait for slave to acknowledge.
	//while (SSPSTATbits.R_W == 1);
	
	i2c_idle();	//wait for the message sending process complete
	
	// If slave does not acknowledge...
	if (SSPCON2bits.ACKSTAT == 1) {
		// Send stop bit.
		SSPCON2bits.PEN = 1;
		while (SSPCON2bits.PEN == 1);
		
		// Set the error flag and exit.
		b_i2c_error_flag = 1;
		return;
	}
	
	// Send stop bit
	SSPCON2bits.PEN = 1;
	while (SSPCON2bits.PEN == 1);
	
	// Clear the error flag.
	b_i2c_error_flag = 0;
}

void i2c_idle(void)
{
	//Need to wait until all ACKEN, RSEN, PEN, RSEN, SEN bit and RW bit is clear
	while((SSPCON2 & 0b00011111) | (SSPSTATbits.R_W));	
}
