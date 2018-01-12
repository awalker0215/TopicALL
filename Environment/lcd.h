#pragma once

#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdio.h>

// Define some device parameters
#define I2C_ADDR   0x3f // I2C device address

// Define some device constants
#define LCD_CHR  1 // Mode - Sending data
#define LCD_CMD  0 // Mode - Sending command

#define LINE1  0x80 // 1st line
#define LINE2  0xC0 // 2nd line
#define LINE3  0x94 // 3rd line
#define LINE4  0xD4 // 4th line

#define LCD_BACKLIGHT_ON   0x08  // On
#define LCD_BACKLIGHT_OFF  0x00  // Off

#define ENABLE  0b00000100 // Enable bit


namespace cm
{
namespace gpio
{

class LCD
{
public:

    LCD()
    {
        fd = wiringPiI2CSetup(I2C_ADDR);
        lcd_init();
        backLightOn ();
    }

    void backLightOn ()
    {
        backlight =  LCD_BACKLIGHT_ON ;
    }

    void backLightOff ()
    {
        backlight =  LCD_BACKLIGHT_OFF;
    }

    // int to string
    void typeInt(int i)
    {
        char array1[20];
        sprintf(array1, "%d",  i);
        typeln(array1);
    }

    // float to string
    void typeFloat(float myFloat)
    {
        char buffer[20];
        sprintf(buffer, "%4.2f",  myFloat);
        typeln(buffer);
    }

// go to location on LCD
    void lcdLoc(int line)
    {
        lcd_byte(line, LCD_CMD);
    }

    // clr lcd go home loc 0x80
    void ClrLcd(void)
    {
        lcd_byte(0x01, LCD_CMD);
        lcd_byte(0x02, LCD_CMD);
    }

// this allows use of any size string
    void typeln(const char *s)
    {
        while ( *s ) lcd_byte(*(s++), LCD_CHR);
    }

    // out char to LCD at current position
    void typeChar(char val)
    {
        lcd_byte(val, LCD_CHR);
    }

private:
    void lcd_init(void)
    {
        // Initialise display
        lcd_byte(0x33, LCD_CMD); // Initialise
        lcd_byte(0x32, LCD_CMD); // Initialise
        lcd_byte(0x06, LCD_CMD); // Cursor move direction
        lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
        lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
        lcd_byte(0x01, LCD_CMD); // Clear display
        delayMicroseconds(500);
    }

    void lcd_byte(int bits, int mode)
    {
        //Send byte to data pins
        // bits = the data
        // mode = 1 for data, 0 for command
        int bits_high;
        int bits_low;
        // uses the two half byte writes to LCD
        bits_high = mode | (bits & 0xF0) | backlight ;
        bits_low = mode | ((bits << 4) & 0xF0) | backlight ;

        // High bits
        wiringPiI2CReadReg8(fd, bits_high);
        lcd_toggle_enable(bits_high);

        // Low bits
        wiringPiI2CReadReg8(fd, bits_low);
        lcd_toggle_enable(bits_low);
    }

    void lcd_toggle_enable(int bits)
    {
        // Toggle enable pin on LCD display
        delayMicroseconds(500);
        wiringPiI2CReadReg8(fd, (bits | ENABLE));
        delayMicroseconds(500);
        wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
        delayMicroseconds(500);
    }

    int  fd;  // seen by all subroutines
    int  backlight;
};

} //namespace utils
} //namespace cm
