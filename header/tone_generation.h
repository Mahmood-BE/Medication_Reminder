#include <math.h> 
//definition of frequecy notes
#define C5 523.25
#define D5 587.33
#define Eb5 622.25
#define E5 659.25
#define F5 698.46
#define G5 783.99
#define Ab5 830.61
#define A5 880.00
#define Bb5 932.33
#define C6 1046.50
#define D6 1174.66
#define Eb6 1244.51
#define E6 1318.51
#define F6 1396.91
#define G6 1567.98
#define Ab6 1661.22
#define A6 1760.00
#define Bb6 1864.66
#define _XTAL_FREQ 8000000


void play_note(double);

void play_note(double note){
    T2CON = 0b00000111;//start timer 2 
    PR2 = (_XTAL_FREQ / (note * 4 * 16));//calculate pr2
    int PWM_dutycycle = PR2/2;//50% duty cycle
    CCP1CON = 0b00001100;//enable PWM 
	CCPR1L = PWM_dutycycle >> 2;//discard 2 LSB and set the duty cycle
}
