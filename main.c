/*
 * File:   main.c
 * Author: Mahmood Abdalwahab
 * ID: 2135135
 * Created on 10 Nov, 2024
 */

#include "main.h"

//Initiate the times of slots
struct Timer T0;
struct Timer T1;
struct Timer T2;
struct Timer T3;

void __interrupt(high_priority) Timer0_ISR(void){//High Priority Interrupt Subroutine
    disable_intRB(); //disable PORTB interrupt
    LATB = 0xF0; //Reset PORTB    
    //Check for the interrupt flag and execute if it si set
    if(INTCONbits.TMR0IF){
        //Load The Timer0 to count 1 second
        TMR0H = 0x0B;
        TMR0L = 0xDC;
        //Check the Time slots and post a notification
        if(BCD_to_Decimal(uc_i2c_rtc_get_hours())==T0.hours){
            if(BCD_to_Decimal(uc_i2c_rtc_get_minutes())==T0.minutes){
                if(BCD_to_Decimal(uc_i2c_rtc_get_seconds())==T0.seconds){
                    LCD_Clear();
                    LCD_String_xy(1, 4, "Time For"); //Display notification
                    LCD_String_xy(2, 2, "Medication 1");
                    LCD_String_xy(4, 0, "Press any button");
                    PORTDbits.RD1 = 1; //Turn ON LED
                    melody_GOT();      //Play the melody
                    PORTDbits.RD1 = 0;//Turn OFF LED
                    pwm_stop();        //Stop playing the melody
                    PORTB;          
                    RBIF = 0;           //Clear PORTB interrupt flag
                    LCD_Clear();
                    LCD_String_xy(2, 2, "Current Time");
                    LCD_String_xy(3, 2, "   :    :   ");
                }
            }
        }
        if(BCD_to_Decimal(uc_i2c_rtc_get_hours())==T1.hours){
            if(BCD_to_Decimal(uc_i2c_rtc_get_minutes())==T1.minutes){
                if(BCD_to_Decimal(uc_i2c_rtc_get_seconds())==T1.seconds){
                    LCD_Clear();
                    LCD_String_xy(1, 4, "Time For");
                    LCD_String_xy(2, 2, "Medication 2");
                    LCD_String_xy(4, 0, "Press any button");
                    PORTDbits.RD1 = 1;
                    melody_GOT();
                    PORTDbits.RD1 = 0;
                    pwm_stop();
                    PORTB;
                    RBIF = 0;
                    LCD_Clear();
                    LCD_String_xy(2, 2, "Current Time");
                    LCD_String_xy(3, 2, "   :    :   ");
                }
            }
        }
        if(BCD_to_Decimal(uc_i2c_rtc_get_hours())==T2.hours){
            if(BCD_to_Decimal(uc_i2c_rtc_get_minutes())==T2.minutes){
                if(BCD_to_Decimal(uc_i2c_rtc_get_seconds())==T2.seconds){
                    LCD_Clear();
                    LCD_String_xy(1, 4, "Time For");
                    LCD_String_xy(2, 2, "Medication 3");
                    LCD_String_xy(4, 0, "Press any button");
                    PORTDbits.RD1 = 1;
                    melody_GOT();
                    PORTDbits.RD1 = 0;
                    pwm_stop();
                    PORTB;
                    RBIF = 0;
                    LCD_Clear();
                    LCD_String_xy(2, 2, "Current Time");
                    LCD_String_xy(3, 2, "   :    :   ");
                }
            }
        }
        if(BCD_to_Decimal(uc_i2c_rtc_get_hours())==T3.hours){
            if(BCD_to_Decimal(uc_i2c_rtc_get_minutes())==T3.minutes){
                if(BCD_to_Decimal(uc_i2c_rtc_get_seconds())==T3.seconds){
                    LCD_Clear();
                    LCD_String_xy(1, 4, "Time For");
                    LCD_String_xy(2, 2, "Medication 4");
                    LCD_String_xy(4, 0, "Press any button");
                    PORTDbits.RD1 = 1;
                    melody_GOT();
                    PORTDbits.RD1 = 0;
                    pwm_stop();
                    PORTB;
                    RBIF = 0;
                    LCD_Clear();
                    LCD_String_xy(2, 2, "Current Time");
                    LCD_String_xy(3, 2, "   :    :   ");
                }
            }
        } 
    }
    //Clear the Flag and return
    INTCONbits.TMR0IF = 0;
    enable_intRB();
}

void __interrupt(low_priority) keypress(void){//Low Priority Interrupt Subroutine
    //Clear LCD
    LCD_Clear();
    //Check for PORTB Flag
    if(RBIF == 1){
        //Disable Interrupt
        disable_intRB();
        //Launch Settings Menu
        settings();
        //Initiate the clock
        LCD_Clear();
        LCD_String_xy(2, 2, "Current Time");
        LCD_String_xy(3, 2, "   :    :   ");
        LATB = 0xF0;
        //Clear PORTB Flag
        PORTB;
        RBIF = 0;
        //Re-Enable Interrupt
        enable_intRB();
    }
}

void main() {
    startup();  //Initiate function
    clock_setup();    //Can be Disabled in simulation as the clock will initial from PC
    LATB = 0xF0;
    LCD_Clear();   //Clear LCD to Prepare for the Clock
    LCD_Command(0x0c);  //Hide Cursor
    LCD_String_xy(2, 2, "Current Time");
    LCD_String_xy(3, 2, "   :    :   ");
    int_init(); //Enable Interrupts
    enable_intRB();
    setup_counter();    //setup the counter
    setup_timer0();     //setup the 4 Medication slots
    setup_timer1();
    setup_timer2();
    setup_timer3();
    clock();    //start the Real-time clock
}

void startup() {
    OSCCON = 0x72; //Set the frequency to 8-Mhz
    CMCON = 0x07; //Comparator OFF
    LCD_Init(); //initialize LCD16x4 in 8-bit mode
    keypad_init(); //initialize 4x3 Keypad
    pwm_initialize();
    __delay_ms(20); //delay
    //Start of EEPROM reading block
    char Data_read;
    char temp_str[5];
    for(int i=0; i<4;i++)    
    {    
        Data_read=EEPROM_Read(i);
        temp_str[i] = Data_read;
    }
    temp_str[4]='\0';
    //End of EEPROM reading block
    strcpy(password, temp_str); //Copy the password into the declared global variable
    if (strcmp(password, "\xff\xff\xff\xff")==0){//Check if password is not saved in EEPROM
        for(int p=0; p<4; p++){
            password[p] = '0';
        }
        EEPROM_WriteString(0, password); //Set password to 0000
        __delay_ms(20);
    }
    TRISDbits.RD1 = 0; //Set RD1 as output for LED 
    LCD_String_xy(2, 3, "Medication"); //Startup Screen 
    LCD_String_xy(3, 4, "Reminder");
    __delay_ms(1000);
}

void clock_setup() {
    LCD_Clear();
    //Start the time clock setup
    LCD_String_xy(1, 2, "Please Enter");
    LCD_String_xy(2, 1, "Current Time in");
    LCD_String_xy(3, 1, "24-Hour Format:");
    LCD_String_xy(4, 2, "00 : 00 : 00");
    LCD_Command(0x0E);
    char i;
    for (i = 0; i < 12; i++) {//shift cursor
        LCD_Command(0x10);
    }
    char secs[5], mins[5], hours[5];
    int myNum, HourH, HourL, Hour, MinH, MinL, Min, SecH, SecL, Sec;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    char lcd_str[8] = "";
L1:
    key = keyfind(); //wait for key input
    myNum = key - '0'; //convert to Integer

    strcpy(lcd_str, (char[2]) { //Copy key pressed as a string instead of a character
        (char) key, '\0'});
    if (within(myNum, 0, 2)) { //check for time conditions and copy the input to its assigned variable
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
    //end of clock setup clock
    Hour = HourH * 10 + HourL;
    Min = MinH * 10 + MinL;
    Sec = SecH * 10 + SecL;
    //save the settings into variables
    i2c_initialize(); //initiate i2c communication
    i2c_rtc_initialize();//Select the slave DS3232
    i2c_rtc_set_seconds(Sec);   //set time in DS3232
    i2c_rtc_set_minutes(Min);
    i2c_rtc_set_hours(Hour);
}

void clock(void) {
    while (1) {//Clock loop
        i2c_initialize(); //start i2c communication
        sec = uc_i2c_rtc_get_seconds(); //get values in BCD format
        min = uc_i2c_rtc_get_minutes();
        hour = uc_i2c_rtc_get_hours();

        BCD_to_String(sec, secs);   //convert from BCD to decimal
        BCD_to_String(min, mins);
        BCD_to_String(hour, hours);

        LCD_String_xy(3, 2, hours); //display time on screen
        LCD_String_xy(3, 7, mins);
        LCD_String_xy(3, 12, secs);
        LCD_String_xy(3, 14, "  ");
        __delay_ms(200);
    }
}

void settings(void) {
    LCD_Clear(); //show settings title
    LCD_String_xy(2, 4, "Settings");
    __delay_ms(1000);
    if(password_check()){return;} //Check password validity
    LCD_Clear();
    LCD_String_xy(1, 0, "1:Add Med"); //show options
    LCD_String_xy(2, 0, "2:Edit Med");
    LCD_String_xy(3, 0, "3:Del Med");
    LCD_String_xy(4, 0, "4:ChangePassword");
    char keypressed = keyfind(); //check the users choice
    while (keypressed != '#') { //return to clock if # is pressed
        
        if(keypressed == '1'){ //move to each choice respective subroutine
            add_med();
            return;
        }else if(keypressed == '2'){
            edit_med();
            return;
        }else if(keypressed == '3'){
            del_med();
            return;
        }else if(keypressed == '4'){
            change_pass();
            return;
        }else {}    //wait for viable choice
        keypressed = keyfind();
    }
}

void setup_counter(){
    T0CON = 0x00;//ensure timer0 is OFF
    T0CONbits.T0PS2 = 1;//set pre-scaler to 1:32 
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS0 = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0CS= 0;
    T0CONbits.T08BIT = 0;
    TMR0H = 0x0B;//Load timer0
    TMR0L = 0xDC;
    INTCONbits.TMR0IE = 1;//enable timer0 interrupt
    INTCONbits.TMR0IF = 0;//clear flag
    T0CONbits.TMR0ON = 1;//turn ON timer0
}

void setup_timer0(){
    char Data_read;//Read EEPROM
    char temp_med1[7];
    for(int i=5; i<11;i++)    
    {    
        Data_read=EEPROM_Read(i);
        temp_med1[i-5] = Data_read;
    }
    __delay_ms(20);
    if(strcmp(temp_med1, "\xff\xff\xff\xff\xff\xff")!= 0){//Check if anything is saved then assign it to a variable
        T0.seconds = (temp_med1[4]-'0')*10+(temp_med1[5]-'0');
        T0.minutes = (temp_med1[2]-'0')*10+(temp_med1[3]-'0');
        T0.hours = (temp_med1[0]-'0')*10+(temp_med1[1]-'0');
    }else {//if nothing is saved assign 99 to all variables
        T0.seconds = 99;
        T0.minutes = 99;
        T0.hours = 99;
    }
}
//same process as previous function for second slot in EEPROM
void setup_timer1(void){
    char Data_read;
    char temp_med2[7];
    for(int j=12; j<18;j++)    
    {    
        Data_read=EEPROM_Read(j);
        temp_med2[j-12] = Data_read;
    }
    __delay_ms(20);
    if(strcmp(temp_med2, "\xff\xff\xff\xff\xff\xff")!= 0){
        T1.seconds = (temp_med2[4]-'0')*10+(temp_med2[5]-'0');
        T1.minutes = (temp_med2[2]-'0')*10+(temp_med2[3]-'0');
        T1.hours = (temp_med2[0]-'0')*10+(temp_med2[1]-'0');
    } else {
        T1.seconds = 99;
        T1.minutes = 99;
        T1.hours = 99;
    }
}
//same process as previous function for third slot in EEPROM
void setup_timer2(void){
    char Data_read;
    char temp_med3[7];
    for(int k=19; k<25;k++)    
    {    
        Data_read=EEPROM_Read(k);
        temp_med3[k-19] = Data_read;
    }
    __delay_ms(20);
    if(strcmp(temp_med3, "\xff\xff\xff\xff\xff\xff")!= 0){
        T2.seconds = (temp_med3[4]-'0')*10+(temp_med3[5]-'0');
        T2.minutes = (temp_med3[2]-'0')*10+(temp_med3[3]-'0');
        T2.hours = (temp_med3[0]-'0')*10+(temp_med3[1]-'0');
    } else {
        T2.seconds = 99;
        T2.minutes = 99;
        T2.hours = 99;        
    }
}
//same process as previous function for fourth slot in EEPROM
void setup_timer3(void){
    char Data_read;
    char temp_med4[7];
    for(int k=26; k<32;k++)    
    {    
        Data_read=EEPROM_Read(k);
        temp_med4[k-26] = Data_read;
    }
    __delay_ms(20);
    if(strcmp(temp_med4, "\xff\xff\xff\xff\xff\xff")!= 0){
        T3.seconds = (temp_med4[4]-'0')*10+(temp_med4[5]-'0');
        T3.minutes = (temp_med4[2]-'0')*10+(temp_med4[3]-'0');
        T3.hours = (temp_med4[0]-'0')*10+(temp_med4[1]-'0');
    } else {
        T3.seconds = 99;
        T3.minutes = 99;
        T3.hours = 99;
    }
}
//sequence for playing the melody
void melody_GOT(void){   
    do{
        for(int i=0; i<3; i++){
            play_note(G6);
            __delay_ms(500);
            play_note(C6);
            __delay_ms(500);
            play_note(Eb6);
            __delay_ms(250);
            play_note(F6);
            __delay_ms(250);
            if(RBIF==1){break;}
        }
        for(int j=0; j<2; j++){
            for(int i=0; i<4; i++){
                play_note(G6);
                __delay_ms(500);
                play_note(C6);
                __delay_ms(500);
                play_note(E6);
                __delay_ms(250);
                play_note(F6);
                __delay_ms(250);
                if(RBIF==1){break;}
            }
            play_note(G6);
            __delay_ms(1500);
        if(RBIF==1){break;}
            play_note(C6);
            __delay_ms(1500);  
        if(RBIF==1){break;}
            play_note(Eb6);
            __delay_ms(375);
            play_note(F6);
            __delay_ms(375);
            play_note(G6);
            __delay_ms(750);
        if(RBIF==1){break;}
            play_note(C6);
            __delay_ms(750);
            play_note(Eb6);
            __delay_ms(375);
            play_note(F6);
            __delay_ms(375);
        if(RBIF==1){break;}
        for(int i=0; i<3; i++){
                play_note(D6);
                __delay_ms(500);
                play_note(G5);
                __delay_ms(500);
                play_note(Bb5);
                __delay_ms(250);
                play_note(C6);
                __delay_ms(250);
                if(RBIF==1){break;}
            }
            play_note(D6);
            __delay_ms(500);
            play_note(G5);
            __delay_ms(500);
            play_note(Bb5);
            __delay_ms(500);
        if(RBIF==1){break;}
            play_note(F6);
            __delay_ms(1500);
        if(RBIF==1){break;}
            play_note(Bb5);
            __delay_ms(1500);
        if(RBIF==1){break;}
            play_note(Eb6);
            __delay_ms(375);
            play_note(D6);
            __delay_ms(375);
            play_note(F6);
            __delay_ms(750);
        if(RBIF==1){break;}
            play_note(Bb5);
            __delay_ms(1500);
        if(RBIF==1){break;}
            play_note(Eb6);
            __delay_ms(250);
            play_note(D6);
            __delay_ms(250);
            play_note(C6);
            __delay_ms(500);
            play_note(Ab5);
            __delay_ms(250);
            play_note(Bb5);
            __delay_ms(250);
        if(RBIF==1){break;}
            for(int i=0; i<3; i++){
                play_note(C6);
                __delay_ms(500);
                play_note(F5);
                __delay_ms(500);
                play_note(Ab5);
                __delay_ms(250);
                play_note(Bb5);
                __delay_ms(250);
                if(RBIF==1){break;}
            }
            play_note(C6);
            __delay_ms(500);
            play_note(F5);
            __delay_ms(500);
            play_note(Ab5);
            __delay_ms(500);
        }
    }while(RBIF==0);
}
