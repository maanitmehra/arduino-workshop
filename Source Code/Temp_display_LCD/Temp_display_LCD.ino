
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

int a;
float temperature;
const int B=3975;                  //B value of the thermistor
float resistance;

void setup() 
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
    lcd.print("Temperature:");

    delay(1000);
}

void loop() 
{
    a = analogRead(0);
    resistance = (float) (1023-a)*10000/a; 
    temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;  
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    
    // print the number of seconds since reset:
    lcd.print(temperature);

    delay(100);
}

