
To use

#include <SPI.h>

// MAX7219 items
#define Number_MAX7219 3        // 3 chips = 24 digits
#define MAX7219_CS  10          // pin the MAX7219's CS pin is connected to
#define Default_Brightness 8    // brightness level 0-15

#include <MAX7219_8_Digit_Driver.h>

// make an instance of MAX7219_Digit called My_Display and set CS pin
MAX7219_8_Digit_Driver My_Display(MAX7219_CS, Number_MAX7219);



To use
// the decimal point does not count against the length of the character string, in this case 24 characters
My_Display.String_To_Buffer("3.14159265358979323846264", Default_Brightness);


Pin connections
Name      Arduino     ESP8266/Wemos D1    MAX7219 7 Segment display
-----     -------     -------             -------------------------
          +5V         5V**                VCC
          GND         GND                 GND
DIN       D11/MOSI    GPIO13 (D7)         DIN
CS        *D7/SS      *GPIO15 (D8)        CS
CLK       D13/SCK     GPIO14 (D5)         CLK
  
* The CS connection is user selectable.
** From the USB 5V supplied to the Wemos D1 Mini PCB, The MAX7219 needs 5V
