
/***************************************************************************************************************************/
/*


      Need some text here

      Pin connections
      Name      Arduino     ESP8266         MAX7219 7 Segment display
      -----     -------     -------         -------------------------
                +5V         5V              VCC
                GND         GND             GND
      DIN       D11/MOSI    GPIO13 (D7)     DIN
      CS        *D10/SS      *GPIO15 (D8)   CS
      CLK       D13/SCK     GPIO14 (D5)     CLK


  Valid characters = 0 to F plus -, H, L, n, P, t, Y, U and c (centigrade) and o to a degree character




*/
/***************************************************************************************************************************/

// Check that the library has not yet been called
#ifndef Max7219_Lib
#define Max7219_Lib

// Check of legacy IDE
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Include needed library and files
#include <SPI.h>

// below is when the library is in the arduino library
//#include <7_Segment_Font.h>

// below is when the library is local
#include "7_Segment_Font.h"

/***************************************************************************************************************************/
/*
  Class for writing to the MAX7219 display
*/
/***************************************************************************************************************************/
class MAX7219_8_Digit_Driver
{

  public:

    // Add a constructor
    MAX7219_8_Digit_Driver(int Pin, int MAX7219_Chips);

    // prototypes of the functions
    void MAX7219_All_Chips(uint8_t Address, uint8_t Data);
    void Write_Digit(uint8_t Register_Address, uint8_t Register_Data);
    void Display_Update(int Brightness);
    void String_To_Buffer(String Message, int Brightness);

  private:

    // Private version of the CS Pin and number of MAX chips in use
    int _Pin;
    int _MAX7219_Chips;

    // sufficient space for 8off MAX7214 drivers or a 64 digit display. An UNO has 2048 bytes available for local variables
    uint8_t _Frame_Buffer[64];

    // MAX7219 register list
    #define MAX7219_Noop          0x0
    #define MAX7219_DecodeMode    0x9
    #define MAX7219_Intensity     0xA
    #define MAX7219_ScanLimit     0xB
    #define MAX7219_Shutdown      0xC
    #define MAX7219_Displaytest   0xF

};
#endif
