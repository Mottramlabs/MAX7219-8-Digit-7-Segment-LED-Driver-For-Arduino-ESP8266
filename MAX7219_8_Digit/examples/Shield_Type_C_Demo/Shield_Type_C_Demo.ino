/*
    simple demo using an UNO and a Type-C Shield
    15th Arpril 2019
    Notes:
    If using the Leonardo board the SPI port is on the 6 pin header not the 10 way header as per the UNO.
    Jumper wires are needed from the 6 pin header for MOSI and CLK.
*/

#include <SPI.h>

// MAX7219 items
#define Number_MAX7219 1        // 3 chips = 24 digits
#define MAX7219_CS  10          // pin the MAX7219's CS pin is connected to
#define Default_Brightness 8    // brightness level 0-15

#include <MAX7219_8_Digit_Driver.h>

// make an instance of MAX7219_Digit called My_Display and set CS pin
MAX7219_8_Digit_Driver My_Display(MAX7219_CS, Number_MAX7219);

// speaker
const int Speaker =  7;
const int Switch_Pin = 4;
int Switch_Last = 0;


// rotary encoder
#include "ESPRotary.h";
#define Rotary_Pin_1   5
#define Rotary_Pin_2   6
#define Moves_Click   4
ESPRotary rot_enc = ESPRotary(Rotary_Pin_1, Rotary_Pin_2, Moves_Click);

int Value = 0;

void setup() {

  Serial.begin(115200); Serial.println(""); Serial.println("Up and Clackin");

  // display test
  My_Display.String_To_Buffer("8.8.8.8.", Default_Brightness);
  delay(1500);

  pinMode(Switch_Pin, INPUT);
  tone(Speaker, 4000, 100);
  rot_enc.setChangedHandler(rotate);

} // end of setup


void loop() {

  rot_enc.loop(); // sample rotary
  Value = rot_enc.getPosition();
  My_Display.String_To_Buffer(String(Value), Default_Brightness);

  // testing the rotary encoders push button, reset the count if so
  if (digitalRead(Switch_Pin) == 0 && Switch_Last == 0) {

    Switch_Last = 1;
    tone(Speaker, 4000, 100);
    rot_enc.resetPosition();

  } // end of switch pushed
  if (digitalRead(Switch_Pin) == 1 && Switch_Last == 1) {

    // clear flag and add a switch debounce delay
    Switch_Last = 0;
    delay(250);

  } // end of switch release test


} // end of loop


void rotate(ESPRotary& rot_enc) {
  Serial.println(rot_enc.getPosition());
  tone(Speaker, 8000, 5);
} // end of function
