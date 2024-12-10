void pwm_initialize(void);
void pwm_uninitialize(void);
void pwm_set_duty_cycle(unsigned int ui_duty_cycle);

void pwm_initialize(void)
{
	CCP1CONbits.CCP1M3 = 1;
	CCP1CONbits.CCP1M2 = 1;		// Configure CCP1 module to operate in PWM mode.
	
	CCP1CONbits.DC1B0 = 0;		// duty cycle = 0% at initial
	CCP1CONbits.DC1B1 = 0;
	CCPR1L = 0x00;	
	
	T2CONbits.TMR2ON = 1;		// Turn on Timer 2
}	

void pwm_stop(void){
    T2CON = 0b00000000;//stop timer 2
}