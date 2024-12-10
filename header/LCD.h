#include <xc.h>
#include "config.h"

#define RS LATC0                    /*PORTD 0 pin is used for Register Select*/
#define EN LATC1                    /*PORTD 1 pin is used for Enable*/
#define ldata LATA                  /*PORTB is used for transmitting data to LCD*/
#define LCD_Port TRISA              /*define macros for PORTB Direction Register*/             
#define LCD_Control TRISC           /*define macros for PORTD Direction Register*/
#define _XTAL_FREQ 8000000

void LCD_Init();
void LCD_Command(char );
void LCD_Char(char x);
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);

/****************************Functions********************************/
void LCD_Init()
{
    __delay_ms(15);           /*15ms,16x4 LCD Power on delay*/
   	LCD_Port = 0x00;       /*Set PORTA as output PORT for LCD data(D0-D7) pins*/
    LCD_Control = 0x00;    /*Set PORTC as output PORT LCD Control(RS,EN) Pins*/
	LCD_Command(0x38);     /*uses 2 line and initialize 5*7 matrix of LCD*/
    LCD_Command(0x01);     /*clear display screen*/
    LCD_Command(0x0c);     /*display on cursor off*/
    LCD_Command(0x06);     /*increment cursor (shift cursor to right)*/
}

void LCD_Clear()
{
    LCD_Command(0x01);     /*clear display screen*/
}

void LCD_Command(char cmd )
{
	ldata= cmd;            /*Send data to PORT as a command for LCD*/   
	RS = 0;                /*Command Register is selected*/
	EN = 1;                /*High-to-Low pulse on Enable pin to latch data*/ 
	NOP();
	EN = 0;
	__delay_ms(3);	
}

void LCD_Char(char dat)
{
	ldata= dat;            /*Send data to LCD*/  
	RS = 1;                /*Data Register is selected*/
	EN=1;                  /*High-to-Low pulse on Enable pin to latch data*/   
	NOP();
	EN=0;
    __delay_ms(1);
}


void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
	  LCD_Char(*msg);
	  msg++;	
    }		
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    switch(row)
    {
        case 1:
            location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
            LCD_Command(location);
            break;
        case 2:
            location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
            LCD_Command(location); 
            break;
        case 3:
            location=(0x90) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
            LCD_Command(location); 
            break;
        case 4:
            location=(0xD0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
            LCD_Command(location); 
            break;
    }
    LCD_String(msg);
}
