/* 
    simple demo using an UNO and 3off 8-digit MAX7219 display boards
    21st March 2019
*/
 
#include <SPI.h>

// MAX7219 items
#define Number_MAX7219 3        // 3 chips = 24 digits
#define MAX7219_CS  10          // pin the MAX7219's CS pin is connected to
#define Default_Brightness 8    // brightness level 0-15

#include <MAX7219_8_Digit_Driver.h>

// make an instance of MAX7219_Digit called My_Display and set CS pin
MAX7219_8_Digit_Driver My_Display(MAX7219_CS, Number_MAX7219);


void setup() {

  Serial.begin(115200); Serial.println(""); Serial.println("Up and Clackin");

  // write data to display plus the brightness level 0-15
  My_Display.String_To_Buffer("   H1   ", Default_Brightness);
  delay(1000);


} // end of setup


void loop() {

  // the decimal point does not count against the length of the character string, in this case 24 characters
  My_Display.String_To_Buffer("3.14159265358979323846264", Default_Brightness);
  delay(1000);

  My_Display.String_To_Buffer("HELL0  ", Default_Brightness);
  delay(1000);


} // end of loop
