#include <xc.h>

void EEPROM_Write(int, char);
char EEPROM_Read(int);
void EEPROM_WriteString(int, char*);

void EEPROM_Write(int address, char data) {
    EEADR = address;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    IPEN = 0;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xaa;
    EECON1bits.WR = 1;
    while (PIR2bits.EEIF == 0);
    PIR2bits.EEIF = 0;
    IPEN = 1;
    INTCONbits.GIE = 1;
}

char EEPROM_Read(int address) {
    EEADR = address;
    EECON1bits.WREN = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    return (EEDATA);
}

void EEPROM_WriteString(int address, char *data) {
    while (*data != 0) {
        EEPROM_Write(address, *data);
        address++;
        *data++;
    }
}
