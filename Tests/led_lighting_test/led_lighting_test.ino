/*****************************************************************************
Title: led_lighting_test
Date: ....
Author: Jerzy Aleksander Gorak
Description:
Testing Program for WS2812B LEDs for Whac-a-cube project.
Program goes through rainbow like change of colours,
where all WS2812B LEDs are being change simultaneously at the same time.
Starts with the red colour,then increases green colour then blue. 
Next, it will start decrasing colours starting with red ending with blue.
It will start with having all of the leds off and will end with all leds off.
******************************************************************************/

#include <Adafruit_NeoPixel.h>
#define LED_IN 2// D2 is recommended for <10khz but will be used at 800khz, as in the project - radio interferance has no effect.
#define LED_COUNT 12 //NeoPixel library is using pixel to describe the leds. therefore pixels=leds, in the context of purpose.
                    // 4 as only one pcb tested.
#define DELAY 5 // time pause for inbetween colour change.
//LEDs set as macros for purpose to set rgb values (colours)
// (refering to the PCB where Ds are the LEDs label)
  // D1 is 0, D2 is 1, D3 is 2, D4 is 3.
  // therefore ; LED Number - 1.- according requiremnets/suggestions of the library 

#define D1 0
#define D2 1
#define D3 2
#define D4 3
#define D5 4
#define D6 5
#define D7 6
#define D8 7
#define D9 8
#define D10 9
#define D11 10
#define D12 11

int pixelFormat = NEO_GRB + NEO_KHZ800;// NeoPixel color format (RGB) & data rate - convenience, just need of typing varable name. 
//required vars for logic increasing and decrasing rgb values
unsigned short int r = 0;
unsigned short int g = 0;
unsigned short int b = 0;

//maybe instead of int use char? smaller size 0-255 which is ideal for the application and saves storage?????????


Adafruit_NeoPixel *pixels; //making variable now for setup function, it then can be accessed globally

/*************************************************************************
  method name : increasing RGB.
  Description: increases the rgb values one by one for red , green and blue.
  and it sets and updates the rgb leds on the pcb.
  final result at the end should be all led flashing white.
*************************************************************************/
void increasingRGB()
{
  //increasing red 
  for(r = 0; r<255; r++)
  {
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels->setPixelColor(D1, pixels->Color(r, g, b));
    pixels->setPixelColor(D2, pixels->Color(r, g, b));
    pixels->setPixelColor(D3, pixels->Color(r, g, b));
    pixels->setPixelColor(D4, pixels->Color(r, g, b));
    pixels->show();//will actually show the colour to the world (visiable changed)
    delay(DELAY);//gives enough time for human eye see the gradual change of colours
  }
  //increasing green 
  for(g = 0; g<255; g++)
  {
    pixels->setPixelColor(D1, pixels->Color(r, g, b));
    pixels->setPixelColor(D2, pixels->Color(r, g, b));
    pixels->setPixelColor(D3, pixels->Color(r, g, b));
    pixels->setPixelColor(D4, pixels->Color(r, g, b));
    pixels->show();
    delay(DELAY);
  }
  //increasing blue
  for(b = 0; b<255; b++)
  {
    pixels->setPixelColor(D1, pixels->Color(r, g, b));
    pixels->setPixelColor(D2, pixels->Color(r, g, b));
    pixels->setPixelColor(D3, pixels->Color(r, g, b));
    pixels->setPixelColor(D4, pixels->Color(r, g, b));
    pixels->show();
    delay(DELAY);
  }
}// END of increasingRGB()


/*************************************************************************
  method name : decrasingRGB.
  Description: does opposite to the increasingRGB method, but it still updates the leds.
  final result at the end should be all leds to be turned off.
*************************************************************************/
void decrasingRGB()
{
  //decrasing red 
  for(r = 255; r>0; r--)
  {
    pixels->setPixelColor(D1, pixels->Color(r, g, b));
    pixels->setPixelColor(D2, pixels->Color(r, g, b));
    pixels->setPixelColor(D3, pixels->Color(r, g, b));
    pixels->setPixelColor(D4, pixels->Color(r, g, b));
    pixels->show();
    delay(DELAY);
  }
  //decrasing green 
  for(g = 255; g>0; g--)
  {
    pixels->setPixelColor(D1, pixels->Color(r, g, b));
    pixels->setPixelColor(D2, pixels->Color(r, g, b));
    pixels->setPixelColor(D3, pixels->Color(r, g, b));
    pixels->setPixelColor(D4, pixels->Color(r, g, b));
    pixels->show();
    delay(DELAY);
  }
  //decrasing blue
  for(b = 255; b>0; b--)
  {
    pixels->setPixelColor(D1, pixels->Color(r, g, b));
    pixels->setPixelColor(D2, pixels->Color(r, g, b));
    pixels->setPixelColor(D3, pixels->Color(r, g, b));
    pixels->setPixelColor(D4, pixels->Color(r, g, b));
    pixels->show();
    delay(DELAY);
  }
}// END of decrasingRGB()


void setup() {
  // creating a new NeoPixel object dynamically with: number of leds in the system, 
  //the LED_IN that data stream will come out from and the pixelFormat variable value
  pixels = new Adafruit_NeoPixel(LED_COUNT, LED_IN, pixelFormat);
  
  pixels->begin(); // initialises NeoPixel strip object 
}

void loop() {
  pixels->clear(); // Sets all leds to being turned off
  delay(100);
  //increasingRGB();
  //decrasingRGB();
  pixels->setPixelColor(D1, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D2, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D3, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D4, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D5, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D6, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D7, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D8, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D9, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D10, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D11, pixels->Color(100, 100, 100));
  pixels->setPixelColor(D12, pixels->Color(100, 100, 100));
  pixels->show();//works for all 3
    
}
