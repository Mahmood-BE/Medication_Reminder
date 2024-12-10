#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include "config.h"
#include "LCD.h"
#include "keypad.h"
#include "I2C_RTC.h"
#include "EEPROM.h"
#include "tone_generation.h"
#include "PWM.h"


#define _XTAL_FREQ 8000000
#define within(value, min, max) (value <= max && value >= min)

typedef struct Timer {
    unsigned int seconds;
    unsigned int minutes;
    unsigned int hours;
};

char key;
char secs[5],mins[5],hours[5];
unsigned char hour, min, sec;
char password[5];

void startup(void);
void clock_setup(void);
void clock(void);
void settings(void);
void int_init(void);
void TMR_ROM_init(void);
void enable_intRB(void);
void disable_intRB(void);
void BCD_to_String(unsigned char, char*);
int BCD_to_Deciaml(int);
int password_check(void);
void add_med(void);
void edit_med(void);
void del_med(void);
void change_pass(void);
void setup_counter(void);
void setup_timer0(void);
void setup_timer1(void);
void setup_timer2(void);
void setup_timer3(void);
void melody_GOT(void);

void int_init(){
    PORTB;
    RBIF = 0;//clear PORTB flag
    GIEH = 1;//enable high priority interrupts
    GIEL = 1;//enable low priority interrupts
    IPEN = 1;//enable interrupt priority
    RBIP = 0;//set PORTB priority to low
    TMR0IP = 1;//set timer0 priority to high
}

void enable_intRB(){
    RBIF = 0; // Enable port (B) Interrupt on change.
    RBIE = 1; // Clear port (B) interrupt flag.
}

void disable_intRB(){
    RBIF = 0; // Enable port (B) Interrupt on change.
    RBIE = 0; // Clear port (B) interrupt flag.
}

void BCD_to_String(unsigned char bcd_value, char *str) {
    str[0] = ((bcd_value >> 4) & 0x0F) + '0'; //isolate upper nibble and convert to character then assign to the array
    str[1] = (bcd_value & 0x0F) + '0'; //isolate lower nibble and convert to character then assign to the array
    str[2] = '\0'; //end of string character
}

int  BCD_to_Decimal(int to_convert){
   return (to_convert >> 4) * 10 + (to_convert & 0x0F); //isolate both nibbles and multiply higher nibble with 10 and add all
}

int password_check(void){
    char temp_pass[5];//temporary variable to store input
    LCD_Clear();//Clear LCD
    LCD_String_xy(2, 1, "Enter Password");//display prompt to user
    LCD_String_xy(3, 3, "_  _  _  _");
    LCD_Command(0x93);//show cursor
    for (int i = 0; i < 4; i++) {//check for input and display starts as password
        char keypressed = keyfind();//save key input
        temp_pass[i] = keypressed;//assign input to its position in password string
        LCD_Char('*');//display a star in password digit
        for (int j = 0; j < 2; j++) {
        LCD_Command(0x14);//shift cursor to next position
        }
    }
    temp_pass[4]='\0';//end of string character
    if (strcmp(temp_pass, password) == 0){//check if password is correct and display prompt return 0
        LCD_Clear();//clear LCD
        LCD_String_xy(2, 0, "Correct Password");//display message
        __delay_ms(1000);//1 sec delay
        return 0;//return 0 and go back to main menu
    } else {//display prompt and return 1  if password is incorrect
        LCD_Clear();//clear LCD
        LCD_String_xy(2, 1, "Wrong Password");//display message
        __delay_ms(1000);//1 sec delay
        return 1;//return 1 and go back to main menu
    }
    
}

void add_med(void){
    //start of block to read EEPROM content
    char slot;//declare variables
    char Data_read;
    char temp_med1[7];
    for(int i=5; i<11;i++)//read slot 1 in EEPROM
    {    
        Data_read=EEPROM_Read(i);
        temp_med1[i-5] = Data_read;
    }
    char temp_med2[7];
    for(int j=12; j<18;j++)//read slot 2 in EEPROM
    {    
        Data_read=EEPROM_Read(j);
        temp_med2[j-12] = Data_read;
    }
    char temp_med3[7];
    for(int k=19; k<25;k++)//read slot 3 in EEPROM    
    {    
        Data_read=EEPROM_Read(k);
        temp_med3[k-19] = Data_read;
    }
    char temp_med4[7];
    for(int m=26; m<32;m++)//read slot 4 in EEPROM    
    {    
        Data_read=EEPROM_Read(m);
        temp_med4[m-26] = Data_read;
    }
    //end of reading block 
    //check all slots if any of them are empty move to next block of code
    if(strcmp(temp_med1, "\xff\xff\xff\xff\xff\xff")==0){//check slot if empty
        goto S2;//go to label S2
    }else if(strcmp(temp_med2, "\xff\xff\xff\xff\xff\xff")==0){//check slot if empty
        goto S2;//go to label S2
    }else if(strcmp(temp_med3, "\xff\xff\xff\xff\xff\xff")==0){//check slot if empty
        goto S2;//go to label S2
    }else if(strcmp(temp_med4, "\xff\xff\xff\xff\xff\xff")==0){//check slot if empty
        goto S2;//go to label S2
    }else{//if all slots are full display a prompt and return to clock
        LCD_Clear();//clear LCD
        LCD_String_xy(2, 1, "All Medication");//display message
        LCD_String_xy(3, 1, "Slots Are Full");
        __delay_ms(1000);//delay 1 sec
        return;
    }
S2:
    LCD_Clear();//display option for user to choose from
    LCD_String_xy(1, 2, "Please Select");//display message
    LCD_String_xy(2, 5, "A Slot");
    LCD_String_xy(3, 2, "[1]      [2]");
    LCD_String_xy(4, 2, "[3]      [4]");
    slot = keyfind();//wait for user input
    LCD_Clear();//clear LCD
    if(slot=='1'){//check if user chose a full slot and display a prompt and return to selection menu
        if(strcmp(temp_med1, "\xff\xff\xff\xff\xff\xff")!=0){LCD_String_xy(2, 1, "Slot is in use");__delay_ms(1000);goto S2;}
    }else if(slot=='2'){
        if(strcmp(temp_med2, "\xff\xff\xff\xff\xff\xff")!=0){LCD_String_xy(2, 1, "Slot is in use");__delay_ms(1000);goto S2;}
    }else if(slot=='3'){
        if(strcmp(temp_med3, "\xff\xff\xff\xff\xff\xff")!=0){LCD_String_xy(2, 1, "Slot is in use");__delay_ms(1000);goto S2;}
    }else if(slot=='4'){
        if(strcmp(temp_med4, "\xff\xff\xff\xff\xff\xff")!=0){LCD_String_xy(2, 1, "Slot is in use");__delay_ms(1000);goto S2;}
    }else if(slot=='#'){
        return;
    }else {goto S2;}//
    LCD_Clear(); //if slot is empty ask the user to fill the details
    LCD_String_xy(1, 2, "Please Enter");
    LCD_String_xy(2, 1, "Medication time");
    LCD_String_xy(3, 1, "24-Hour Format:");
    LCD_String_xy(4, 2, "00 : 00 : 00");
    LCD_Command(0x0E);
    char i;//block to save user input to variables after checking time conditions
    for (i = 0; i < 12; i++) {
        LCD_Command(0x10);
    }
    char secs[5], mins[5], hours[5];//declare variables
    int myNum, HourH, HourL, Hour, MinH, MinL, Min, SecH, SecL, Sec;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    char lcd_str[8] = "";
L1:
    key = keyfind();//check for input
    myNum = key - '0';//convert to integer

    strcpy(lcd_str, (char[2]) {//copy key into LCD string
        (char) key, '\0'});
    if (within(myNum, 0, 2)) {
        LCD_String_xy(4, 2, lcd_str);
        HourH = myNum;
        if (myNum == 2) {
L2:
            key = keyfind();
            myNum = key - '0';

            strcpy(lcd_str, (char[2]) {
                (char) key, '\0'});
            if (within(myNum, 0, 3)) {
                HourL = myNum;
                LCD_String_xy(4, 3, lcd_str);
            } else {
                goto L2;
            }
        } else {
L3:
            key = keyfind();
            myNum = key - '0';

            strcpy(lcd_str, (char[2]) {
                (char) key, '\0'});
            if (within(myNum, 0, 9)) {
                HourL = myNum;
                LCD_String_xy(4, 3, lcd_str);
            } else {
                goto L3;
            }
        }

    } else {
        goto L1;
    }
    for (i = 0; i < 3; i++) {
        LCD_Command(0x14);
    }
L4:
    key = keyfind();
    myNum = key - '0';

    strcpy(lcd_str, (char[2]) {
        (char) key, '\0'});
    if (within(myNum, 0, 5)) {
        LCD_String_xy(4, 7, lcd_str);
        MinH = myNum;
L5:
        key = keyfind();
        myNum = key - '0';

        strcpy(lcd_str, (char[2]) {
            (char) key, '\0'});
        if (within(myNum, 0, 9)) {
            LCD_String_xy(4, 8, lcd_str);
            MinL = myNum;
        } else {
            goto L5;
        }

    } else {
        goto L4;
    }
    for (i = 0; i < 3; i++) {
        LCD_Command(0x14);
    }
L6:
    key = keyfind();
    myNum = key - '0';

    strcpy(lcd_str, (char[2]) {
        (char) key, '\0'});
    if (within(myNum, 0, 5)) {
        LCD_String_xy(4, 12, lcd_str);
        SecH = myNum;
L7:
        key = keyfind();
        myNum = key - '0';

        strcpy(lcd_str, (char[2]) {
            (char) key, '\0'});
        if (within(myNum, 0, 9)) {
            LCD_String_xy(4, 13, lcd_str);
            SecL = myNum;
        } else {
            goto L7;
        }

    } else {
        goto L6;
    }
    LCD_Clear();
    LCD_Command(0x0c);

    Hour = HourH * 10 + HourL;
    Min = MinH * 10 + MinL;
    Sec = SecH * 10 + SecL;
    char new_med_time[8];
    sprintf(new_med_time, "%02d%02d%02d", Hour, Min, Sec);//write user input to a string
    switch (slot){//save the time in the selected slot in the EEPROM
        case '1':
            EEPROM_WriteString(5, new_med_time);
            __delay_ms(20);//delay
            setup_timer0();//setup the interrupt parameter
            return;
        case '2':
            EEPROM_WriteString(12, new_med_time);
            __delay_ms(20);
            setup_timer1();//setup the interrupt parameter
            return;
        case '3':
            EEPROM_WriteString(19, new_med_time);
            __delay_ms(20);
            setup_timer2();//setup the interrupt parameter
            return;
        case '4':
            EEPROM_WriteString(26, new_med_time);
            __delay_ms(20);
            setup_timer3();//setup the interrupt parameter
            return;
    }    
}

void edit_med(void){
    char slot;//reading all assigned slots in EEPROM
    char Data_read;
    char temp_med1[7];
    for(int i=5; i<11;i++)    
    {    
        Data_read=EEPROM_Read(i);
        temp_med1[i-5] = Data_read;
    }
    char temp_med2[7];
    for(int j=12; j<18;j++)    
    {    
        Data_read=EEPROM_Read(j);
        temp_med2[j-12] = Data_read;
    }
    char temp_med3[7];
    for(int k=19; k<25;k++)    
    {    
        Data_read=EEPROM_Read(k);
        temp_med3[k-19] = Data_read;
    }
    char temp_med4[7];
    for(int m=26; m<32;m++)    
    {    
        Data_read=EEPROM_Read(m);
        temp_med4[m-26] = Data_read;
    }

    if(strcmp(temp_med1, "\xff\xff\xff\xff\xff\xff")!=0){//checking whether a slot is occupied or not in the EEPROM
        goto S2;
    }else if(strcmp(temp_med2, "\xff\xff\xff\xff\xff\xff")!=0){
        goto S2;
    }else if(strcmp(temp_med3, "\xff\xff\xff\xff\xff\xff")!=0){
        goto S2;
    }else if(strcmp(temp_med4, "\xff\xff\xff\xff\xff\xff")!=0){
        goto S2;
    }else{    
        LCD_Clear();
        LCD_String_xy(2, 2, "No Medication");
        LCD_String_xy(3, 1, "Slots Are in use");
        __delay_ms(1000);
        return;
    }
S2:
    LCD_Clear();//prompt the user to select a slot to edit
    LCD_String_xy(1, 2, "Please Select");
    LCD_String_xy(2, 5, "A Slot");
    LCD_String_xy(3, 2, "[1]      [2]");
    LCD_String_xy(4, 2, "[3]      [4]");    
    slot = keyfind();
    LCD_Clear();
    if(slot=='1'){//is slot is empty show a message and return
        if(strcmp(temp_med1, "\xff\xff\xff\xff\xff\xff")==0){LCD_String_xy(2, 1, "Slot Not in Use");__delay_ms(1000);goto S2;}
    }else if(slot=='2'){
        if(strcmp(temp_med2, "\xff\xff\xff\xff\xff\xff")==0){LCD_String_xy(2, 1, "Slot Not in Use");__delay_ms(1000);goto S2;}
    }else if(slot=='3'){
        if(strcmp(temp_med3, "\xff\xff\xff\xff\xff\xff")==0){LCD_String_xy(2, 1, "Slot Not in Use");__delay_ms(1000);goto S2;}
    }else if(slot=='4'){
        if(strcmp(temp_med4, "\xff\xff\xff\xff\xff\xff")==0){LCD_String_xy(2, 1, "Slot Not in Use");__delay_ms(1000);goto S2;}
    }else if(slot=='#'){
    }else {goto S2;}
    LCD_Clear();//prompt the user to enter the new time for the slot
    LCD_String_xy(1, 2, "Please Enter");
    LCD_String_xy(2, 1, "Medication time");
    LCD_String_xy(3, 1, "24-Hour Format:");
    LCD_String_xy(4, 2, "00 : 00 : 00");
    LCD_Command(0x0E);
    char i;//start the keypad assignment routine and save the new time in their respective variables
    for (i = 0; i < 12; i++) {
        LCD_Command(0x10);
    }
    char secs[5], mins[5], hours[5];
    int myNum, HourH, HourL, Hour, MinH, MinL, Min, SecH, SecL, Sec;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    char lcd_str[8] = "";
L1:
    key = keyfind();
    myNum = key - '0';

    strcpy(lcd_str, (char[2]) {
        (char) key, '\0'});
    if (within(myNum, 0, 2)) {
        LCD_String_xy(4, 2, lcd_str);
        HourH = myNum;
        if (myNum == 2) {
L2:
            key = keyfind();
            myNum = key - '0';

            strcpy(lcd_str, (char[2]) {
                (char) key, '\0'});
            if (within(myNum, 0, 3)) {
                HourL = myNum;
                LCD_String_xy(4, 3, lcd_str);
            } else {
                goto L2;
            }
        } else {
L3:
            key = keyfind();
            myNum = key - '0';

            strcpy(lcd_str, (char[2]) {
                (char) key, '\0'});
            if (within(myNum, 0, 9)) {
                HourL = myNum;
                LCD_String_xy(4, 3, lcd_str);
            } else {
                goto L3;
            }
        }

    } else {
        goto L1;
    }
    for (i = 0; i < 3; i++) {
        LCD_Command(0x14);
    }
L4:
    key = keyfind();
    myNum = key - '0';

    strcpy(lcd_str, (char[2]) {
        (char) key, '\0'});
    if (within(myNum, 0, 5)) {
        LCD_String_xy(4, 7, lcd_str);
        MinH = myNum;
L5:
        key = keyfind();
        myNum = key - '0';

        strcpy(lcd_str, (char[2]) {
            (char) key, '\0'});
        if (within(myNum, 0, 9)) {
            LCD_String_xy(4, 8, lcd_str);
            MinL = myNum;
        } else {
            goto L5;
        }

    } else {
        goto L4;
    }
    for (i = 0; i < 3; i++) {
        LCD_Command(0x14);
    }
L6:
    key = keyfind();
    myNum = key - '0';

    strcpy(lcd_str, (char[2]) {
        (char) key, '\0'});
    if (within(myNum, 0, 5)) {
        LCD_String_xy(4, 12, lcd_str);
        SecH = myNum;
L7:
        key = keyfind();
        myNum = key - '0';

        strcpy(lcd_str, (char[2]) {
            (char) key, '\0'});
        if (within(myNum, 0, 9)) {
            LCD_String_xy(4, 13, lcd_str);
            SecL = myNum;
        } else {
            goto L7;
        }

    } else {
        goto L6;
    }
    LCD_Clear();
    LCD_Command(0x0c);

    Hour = HourH * 10 + HourL;//convert digits to number and store them in variables
    Min = MinH * 10 + MinL;
    Sec = SecH * 10 + SecL;
    char new_med_time[8];
    sprintf(new_med_time, "%02d%02d%02d%c", Hour, Min, Sec);
    switch (slot){//write the time in the selected spot in EEPROM
        case '1':
            EEPROM_WriteString(5, new_med_time);
            __delay_ms(20);
            setup_timer0();
            return;
        case '2':
            EEPROM_WriteString(12, new_med_time);
            __delay_ms(20);
            setup_timer1();
            return;
        case '3':
            EEPROM_WriteString(19, new_med_time);
            __delay_ms(20);
            setup_timer2();
            return;
        case '4':
            EEPROM_WriteString(26, new_med_time);
            __delay_ms(20);
            setup_timer3();
            return;
    }
}

void del_med(void){//
D1:
    LCD_Clear();//prompt the use to delete a slot 
    LCD_String_xy(1, 2, "Delete Med:");
    LCD_String_xy(3, 2, "[1]      [2]");
    LCD_String_xy(4, 2, "[3]      [4]");
    char choice;
    char Data_read;
    char temp_str[8];
    choice = keyfind();//wait for user input
    if (choice=='1'){//check the input
    for(int i1=5; i1 < 11; i1++){//read the location in the EEPROM and check whether the slot is occupied or not
        Data_read=EEPROM_Read(i1);
        temp_str[i1-5] = Data_read;
    }
        LCD_Clear();
        if (strcmp(temp_str, "\xff\xff\xff\xff\xff\xff")==0){
            LCD_String_xy(2, 4, "No Saved");
            LCD_String_xy(3, 3, "Medication");
            __delay_ms(1000);
            return;
            } else {
                LCD_String_xy(2, 4, "Confirm?");//ask the use rto confirm the process
                LCD_String_xy(3, 1, "1:Yes     2:No");
D2:
                choice = keyfind();
                if(choice=='1'){
                    EEPROM_WriteString(5, "\xff\xff\xff\xff\xff\xff");// the EEPROM location
                    setup_timer0();
                } else if(choice=='2'){
                    return;
                } else {goto D2;}
            }
    } else if(choice=='2'){
    for(int i2=12; i2 < 18; i2++){
        Data_read=EEPROM_Read(i2);
        temp_str[i2-12] = Data_read;
    }
        LCD_Clear();
        if (strcmp(temp_str, "\xff\xff\xff\xff\xff\xff")==0){
            LCD_String_xy(2, 4, "No Saved");
            LCD_String_xy(3, 3, "Medication");
            __delay_ms(1000);
            return;
            } else {
                LCD_String_xy(2, 4, "Confirm?");
                LCD_String_xy(3, 1, "1:Yes     2:No");
D3:
                choice = keyfind();
                if(choice=='1'){
                    EEPROM_WriteString(12, "\xff\xff\xff\xff\xff\xff");
                    setup_timer1();
                } else if(choice=='2'){
                    return;
                } else {goto D3;}
            }  
    } else if(choice=='3'){
    for(int i3=19; i3 < 25; i3++){
        Data_read=EEPROM_Read(i3);
        temp_str[i3-19] = Data_read;
    }
        LCD_Clear();
        if (strcmp(temp_str, "\xff\xff\xff\xff\xff\xff")==0){
            LCD_String_xy(2, 4, "No Saved");
            LCD_String_xy(3, 3, "Medication");
            __delay_ms(1000);
            return;
            } else {
                LCD_String_xy(2, 4, "Confirm?");
                LCD_String_xy(3, 1, "1:Yes     2:No");
D4:
                choice = keyfind();
                if(choice=='1'){
                    EEPROM_WriteString(19, "\xff\xff\xff\xff\xff\xff");
                    setup_timer2();
                } else if(choice=='2'){
                    return;
                } else {goto D4;}
            }         
    } else if(choice=='4'){
    for(int i4=26; i4 < 32; i4++){
        Data_read=EEPROM_Read(i4);
        temp_str[i4-26] = Data_read;
    }
        LCD_Clear();
        if (strcmp(temp_str, "\xff\xff\xff\xff\xff\xff")==0){
            LCD_String_xy(2, 4, "No Saved");
            LCD_String_xy(3, 3, "Medication");
            __delay_ms(1000);
            return;
            } else {
                LCD_String_xy(2, 4, "Confirm?");
                LCD_String_xy(3, 1, "1:Yes     2:No");
DH:
                choice = keyfind();
                if(choice=='1'){
                    EEPROM_WriteString(26, "\xff\xff\xff\xff\xff\xff\xff");
                    setup_timer3();
                } else if(choice=='2'){
                    return;
                } else {goto DH;}
            }   
    }else if(choice=='#'){
        return;
    }else {goto D1;}
}

void change_pass(void){
    char temp_pass[5];
    char keypressed;
    LCD_Clear();//prompt the user to enter the new password
    LCD_String_xy(1, 3, "Enter New");
    LCD_String_xy(2, 4, "Password");
    LCD_String_xy(3, 3, "_  _  _  _");
    LCD_Command(0x93);//show the cursor
    for (int i = 0; i < 4; i++) {
        keypressed = keyfind();//wait for keyinput
        if(keypressed=='#'){//return to the main menu if # is pressed
            return;
        }
        temp_pass[i] = keypressed;
        LCD_Char(keypressed);
        for (int j = 0; j < 2; j++) {//shift the cursor
        LCD_Command(0x14);
        }
    }
    temp_pass[4] = '\0';
    LCD_Clear();
    LCD_String_xy(2, 4, "Confirm?");//prompt the user to confirm the choice
    LCD_String_xy(3, 1, "1:Yes     2:No");
P1:
    keypressed = keyfind();//wait for user input
    if(keypressed == '1'){//user confirms the action and new password is saved to EEPROM
        LCD_Clear();
        LCD_String_xy(2, 1, "Confirmed");
        strcpy(password, temp_pass);
        EEPROM_WriteString(0, password);
        __delay_ms(20);
    }else if(keypressed == '2'){//if user does not confirm return to main menu
        return;
    }else {goto P1;}
}
