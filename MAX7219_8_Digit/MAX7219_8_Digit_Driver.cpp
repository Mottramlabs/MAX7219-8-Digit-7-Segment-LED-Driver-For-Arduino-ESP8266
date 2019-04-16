#include "Arduino.h"
#include <SPI.h>
#include "MAX7219_8_Digit_Driver.h"


// ------------------------------------------------------------------------------------------------------------------------
// constructor
// ------------------------------------------------------------------------------------------------------------------------
MAX7219_8_Digit_Driver::MAX7219_8_Digit_Driver(int Pin, int MAX7219_Chips) {

  // set load pin to output
  pinMode(Pin, OUTPUT);

  // local variables
  _Pin = Pin;
  _MAX7219_Chips = MAX7219_Chips;

  // reverse the SPI transfer to send the MSB first
  SPI.setBitOrder(MSBFIRST);

  // start SPI, SPI slow on ESP8266 https://github.com/esp8266/Arduino/issues/2624
  SPI.begin();

  digitalWrite(_Pin, HIGH);  // Take CS High

#if defined(__AVR_ATmega168__) ||defined(__AVR_ATmega168P__) ||defined(__AVR_ATmega328P__) ||defined(__AVR_ATmega32U4__)
  Serial.println("Hello UNO");
#else
  Serial.println("Hello ESP8266");
  // only needed for ESP8266, does not work with UNO, the larger the number the faster the SPI
  // 4000000L gives a similar purformance speed to an UNO, 15000000L is much too fast for the MAX7219
  // 8000000L updates a 24 digit display in 750uS
  SPI.setFrequency(8000000L);
#endif

} // end of function


// ------------------------------------------------------------------------------------------------------------------------
// write data to all MAX7219 chips present, used to initialize display or set brightness
// ------------------------------------------------------------------------------------------------------------------------
void MAX7219_8_Digit_Driver::MAX7219_All_Chips(uint8_t Address, uint8_t Data) {

  // take CS low
  digitalWrite(_Pin, LOW);

  for (uint8_t i = 0; i < _MAX7219_Chips; ++i) {

    SPI.transfer(Address);      //  address
    SPI.transfer(Data);         // data value

  } // end of loop

  // return CS high
  digitalWrite(_Pin, HIGH);

} // end Function


// ------------------------------------------------------------------------------------------------------------------------
// write a character to a MAX7219 chip to one of the 8 locations. Write a Noop to all other MAX7219 chips
// ------------------------------------------------------------------------------------------------------------------------
void MAX7219_8_Digit_Driver::Write_Digit(uint8_t Register_Address, uint8_t Register_Data) {

  // range check the address value
  if (Register_Address >= 0 && Register_Address <= ((_MAX7219_Chips * 8) - 1)) {
    // range check passed. Which MAX7219 chip to write data to, write NOOP's to all others

    // find the chip number you are writing to
    int Chip_Number = Register_Address / 8;

    // find the chips data register to write data too, result is Register_Address. Range = 1 to 8 (data registers of the MAX7219)
    int yy = (Chip_Number * 8); Register_Address = (Register_Address - yy) + 1;

    // start the transfer to the Max chips, loop through all MAX chips, writing either the data or NOOP
    digitalWrite(_Pin, LOW);

    // test which chip to write data too and write noop to all others, that is register address 0x00
    // loop from 0 to the Number of MAX7219 chip (far right to far left)
    for (int xx = 0; xx < _MAX7219_Chips; xx++) {

      // test if it the chip we are to write data to else write a NOOP
      if (xx == Chip_Number) {
        // this is the chip required, now select the data register and write the data

        // test if decimal point flag bit is set
        if (bitRead(Register_Data, 7) == 1) {

          // remove the dp flag and use the lookup table, then set the decimal point LED bit
          Register_Data = pgm_read_byte(&Char_Set[Register_Data - 128]);
          Register_Data = Register_Data + 128;

        } // end dp set

        // no decimal point set
        else {
          // range check the data to write, if too large for the lookup table then turn all segments off
          if (Register_Data < 0x1F) {
            // use the lookup table to find the character to display
            Register_Data = pgm_read_byte(&Char_Set[Register_Data]);
          }
          else {
            // out of range so blank the digit
            Register_Data = 0x00;
          }
        } // end no dp set

        // write the address and data to the SPI port
        SPI.transfer(Register_Address); SPI.transfer(Register_Data);

      }

      // else write noop to other MAX7219 chips
      else {

        // write the address and data to the SPI port
        SPI.transfer(MAX7219_Noop); SPI.transfer(0x00);

      } // end of else

    } // end of loop

    // return CS high
    digitalWrite(_Pin, HIGH);

  } // end of range check

} // end of function


// ------------------------------------------------------------------------------------------------------------------------
// update the display with the data in the Frame_Buffer, note that Frame_Buffer[0] is far right of the display
// ------------------------------------------------------------------------------------------------------------------------
void MAX7219_8_Digit_Driver::Display_Update(int Brightness) {

  // range check the brightess level, valid 0-15
  if (Brightness < 0 || Brightness > 15) {

    Brightness = 8;

  } // end range check

  for (int i = 0; i <  (_MAX7219_Chips * 8); i++) {

    Write_Digit(i, _Frame_Buffer[i]); // Write to column, with data

  } // end of loop

  // write to all MAX7219 chip control registers, only adds a little time but corrects any MAX7219 sync errors
  // timing using an UNO adds 130uS at the most
  MAX7219_All_Chips(MAX7219_DecodeMode, 0x00);
  MAX7219_All_Chips(MAX7219_Intensity, Brightness);
  MAX7219_All_Chips(MAX7219_ScanLimit, 0x07);
  MAX7219_All_Chips(MAX7219_Displaytest, 0x00);
  MAX7219_All_Chips(MAX7219_Shutdown, 0x01);

} // end of function


// ------------------------------------------------------------------------------------------------------------------------
// write a string message to the data buffer
// ------------------------------------------------------------------------------------------------------------------------
void MAX7219_8_Digit_Driver::String_To_Buffer(String Message, int Brightness) {

  // clear the data buffer
  for (int a = 0; a < (_MAX7219_Chips * 8); a++) {
    // blank digit
    _Frame_Buffer[a] = 0x1A;
  } // end of loop

  // define a buffer pointer and decimal point flag
  int Buffer_Pointer = 0; int DP_Flag = 0;

  //  for (int ll = 0; ll < My_Message.length(); ll++) {
  for (int ll = (Message.length() - 1); ll > -1; ll--) {

    // test of ascii value is a decimal point
    if (Message.charAt(ll) == 0x2E) {
      DP_Flag = 1;  // set flag
    } // end if

    // the character is not a dp
    else {

      // start with a blank digit
      int Value = 0x1A;

      // test if numbers are 0-9
      if (Message.charAt(ll) > 0x2F && Message.charAt(ll) < 0x3A) {
        Value = (Message.charAt(ll) - 0x30);
      }

      // test if numbers are A-F
      if (Message.charAt(ll) > 0x40 && Message.charAt(ll) < 0x47) {
        Value = (Message.charAt(ll) - 0x37);
      }

      // special characters. -, H, L, n, P, t, Y, U and c (centigrade) and o to a degree character
      // minus sign
      if (Message.charAt(ll) == 0x2D) {
        Value = 0x10;
      }

      // c for centigrade
      if (Message.charAt(ll) == 0x63) {
        Value = 0x11;
      }

      // o for degree
      if (Message.charAt(ll) == 0x6F) {
        Value = 0x12;
      }


      // Space
      if (Message.charAt(ll) == 0x20) {
        Value = 0x1A;
      }

      // H
      if (Message.charAt(ll) == 0x48 || Message.charAt(ll) == 0x68) {
        Value = 0x13;
      }

      // L
      if (Message.charAt(ll) == 0x4C || Message.charAt(ll) == 0x6C) {
        Value = 0x14;
      }

      // n
      if (Message.charAt(ll) == 0x4E || Message.charAt(ll) == 0x6E) {
        Value = 0x15;
      }

      // P
      if (Message.charAt(ll) == 0x50 || Message.charAt(ll) == 0x70) {
        Value = 0x16;
      }

      // t
      if (Message.charAt(ll) == 0x54 || Message.charAt(ll) == 0x74) {
        Value = 0x17;
      }

      // Y
      if (Message.charAt(ll) == 0x59 || Message.charAt(ll) == 0x79) {
        Value = 0x18;
      }

      // U
      if (Message.charAt(ll) == 0x55 || Message.charAt(ll) == 0x75) {
        Value = 0x19;
      }

      // range check, only incrument the pointer if not over range
      if (Buffer_Pointer < (_MAX7219_Chips * 8)) {

        if (DP_Flag == 0) {
          _Frame_Buffer[Buffer_Pointer] = Value;
        }
        else {
          _Frame_Buffer[Buffer_Pointer] = Value + 0x80;
          DP_Flag = 0;  // clear flag
        }

        // buffer at location Buffer_Pointer now has the location within the lookup
        // table for the character, if bit 7 set then a d.p. should be added
        Buffer_Pointer++;
      } // end range check

    } // end else

  } // end message read

  Display_Update(Brightness);

}  // end function
