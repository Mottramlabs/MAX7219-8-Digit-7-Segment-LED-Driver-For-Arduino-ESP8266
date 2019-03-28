# MAX7219 8 Digit 7 Segment Driver
A simple library for driving the cheap 8 digit 7 segments LED boards found on eBay. With these boards the first digit is on the far right of the display (MAX7219 Register1) and the last digit is on the far left (Register 8).
This library supports cascading several of these boards to make a large multi-digit display, or many individual displays all driven from the same I/O pins and library.
Select the number of MAX7219 chips used (one per 8 digits) then simply write a string of characters to the display, add spaces to the string to space out the displays digits as required. Decimal points do not count as a character as far as the function is concerned but the function does light up the decimal points. 
So you could have a panel made from several of these 8 digit displays and then write single string to all of the displays with one command, using spaces to move the information to the required display. I.e. one display could display time and another display the date, use spaces to control where the data appears on those displays.

