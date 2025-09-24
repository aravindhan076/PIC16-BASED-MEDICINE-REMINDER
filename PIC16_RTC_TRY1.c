// ================= PIC16F877A CONFIG =================
#pragma config FOSC = HS        // HS oscillator
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = OFF      // Power-up Timer disabled
#pragma config BOREN = OFF      // Brown-out Reset disabled
#pragma config LVP = OFF        // Low-voltage programming disabled
#pragma config CPD = OFF        // Data EEPROM protection off
#pragma config WRT = OFF        // Flash write protection off
#pragma config CP = OFF         // Code protection off

#include <xc.h>
#define _XTAL_FREQ 20000000

// ================= LCD PINS =================
#define LCD PORTD
#define RS  RB0
#define RW  RB1
#define EN  RB2

// ================= LED PINS =================
#define LED1 RA0
#define LED2 RA1
#define LED3 RA2
// ================= BUTTON00000 PINS =================
#define BTN RA3

// ================= RTC SPI =================
#define CS RC0

// ================= FUNCTION PROTOTYPES =================
// EEPROM
void EEPROM_Write(unsigned char addr, unsigned char data);
unsigned char EEPROM_Read(unsigned char addr);
void EEPROM_WriteString(unsigned char start_addr, const char *str);
void EEPROM_ReadString(unsigned char start_addr, char *buf, unsigned char maxlen);

// LCD
void lcd_init(void);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_print(const char *str);

// RTC
unsigned char bcd_to_dec(unsigned char data);
unsigned char dec_to_bcd(unsigned char data);
unsigned char spi_rw(unsigned char data);
void ds3234_set_time(unsigned char h, unsigned char m, unsigned char s);
void ds3234_read_time(unsigned char *h, unsigned char *m, unsigned char *s);

// Utility
void delay_seconds(unsigned char sec);

// ================= MAIN =================
void main() {
    unsigned char hours, minutes, seconds;
    char buffer[16];

    // ======== PORT INIT ========
    TRISD = 0x00; // LCD data
    TRISB = 0x00; // LCD control
    TRISA = 0x00; // LEDs
    TRISC0 = 0;   // CS pin
    CS = 1;

    // SPI for RTC
    TRISC3 = 0; // SCK
    TRISC4 = 1; // SDI
    TRISC5 = 0; // SDO
    SSPSTAT = 0x00;
    SSPCON = 0x20;

    lcd_init();

    // ======== EEPROM MEDICINE NAMES ========
    EEPROM_WriteString(0x00, "PARACITAMOL");
    EEPROM_WriteString(0x20, "ASPIRIN");
    EEPROM_WriteString(0x40, "IBUPROFEN");

    // ======== RTC INITIAL TIME ========
    ds3234_set_time(14, 49, 0);

    while(1) {
        // ======== READ TIME ========
        ds3234_read_time(&hours, &minutes, &seconds);

        // ======== LCD FIRST LINE FIXED ========
        lcd_cmd(0x80);
        lcd_print("  MEDICINE BOX   ");

        // ======== CLEAR SECOND LINE ========
        lcd_cmd(0xC0);
        lcd_print("                ");

        // ======== MEDICINE SCHEDULE ========
        if(hours==14 && minutes==50) {           // MEDICINE 1
            EEPROM_ReadString(0x00, buffer, 16);
            LED1 = 1; lcd_cmd(0xC0); lcd_print(buffer);
            delay_seconds(60); // show for 1 min
            LED1 = 0;
        }
        else if(hours==14 && minutes==51) {      // MEDICINE 2
            EEPROM_ReadString(0x20, buffer, 16);
            LED2 = 1; lcd_cmd(0xC0); lcd_print(buffer);
            delay_seconds(60);
            LED2 = 0;
        }
        else if(hours==14 && minutes==52) {       // MEDICINE 3
            EEPROM_ReadString(0x40, buffer, 16);
            LED3 = 1; lcd_cmd(0xC0); lcd_print(buffer);
            delay_seconds(60);
            LED3 = 0;
        }

        __delay_ms(500);
    }
}

// ================= UTILITY =================
void delay_seconds(unsigned char sec){
    unsigned char i;
    for(i=0;i<sec;i++){
        __delay_ms(1000);
    }
}

// ================= EEPROM =================
void EEPROM_Write(unsigned char addr, unsigned char data){
    EEADR = addr;
    EEDATA = data;
    EEPGD = 0;
    WREN = 1;

    
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    

    while(!PIR2bits.EEIF);
    EEIF = 0;
    WREN = 0;
}

unsigned char EEPROM_Read(unsigned char addr){
    EEADR = addr;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    return EEDATA;
}

void EEPROM_WriteString(unsigned char start_addr, const char *str){
    while(*str) EEPROM_Write(start_addr++, *str++);
    EEPROM_Write(start_addr, '\0');
}

void EEPROM_ReadString(unsigned char start_addr, char *buf, unsigned char maxlen){
    unsigned char i=0,c;
    do{
        c=EEPROM_Read(start_addr++);
        buf[i++] = c;
    } while(c!='\0' && i<maxlen-1);
    buf[i]='\0';
}

// ================= LCD =================
void lcd_cmd(unsigned char cmd){
    LCD = cmd; RS=0; RW=0; EN=1;
    __delay_ms(2); EN=0;
}

void lcd_data(unsigned char data){
    LCD = data; RS=1; RW=0; EN=1;
    __delay_ms(2); EN=0;
}

void lcd_init(void){
    TRISD = 0x00; TRISB0=0; TRISB1=0; TRISB2=0;
    lcd_cmd(0x38); lcd_cmd(0x0C); lcd_cmd(0x01); lcd_cmd(0x06);
}

void lcd_print(const char *str){
    while(*str) lcd_data(*str++);
}

// ================= RTC =================
unsigned char spi_rw(unsigned char data){
    SSPBUF = data;
    while(!SSPIF);
    SSPIF = 0;
    return SSPBUF;
}

unsigned char bcd_to_dec(unsigned char data){
    return ((data>>4)*10) + (data & 0x0F);
}

unsigned char dec_to_bcd(unsigned char data){
    return (unsigned char)(((data/10)<<4) | (data%10));
}

void ds3234_set_time(unsigned char h, unsigned char m, unsigned char s){
    CS=0;
    spi_rw(0x80); // seconds register write
    spi_rw(dec_to_bcd(s));
    spi_rw(dec_to_bcd(m));
    spi_rw(dec_to_bcd(h));
    CS=1;
}

void ds3234_read_time(unsigned char *h, unsigned char *m, unsigned char *s){
    CS=0;
    spi_rw(0x00); // seconds register read
    *s = bcd_to_dec(spi_rw(0xFF));
    *m = bcd_to_dec(spi_rw(0xFF));
    *h = bcd_to_dec(spi_rw(0xFF));
    CS=1;
}