/*
Title:Press_test
Description:
when magnet is near the hall effect sensor it will be registered as a button press and so 
conequently light up corresponding LED.
Author: Jerzy Aleksander Gorak
 */
#include <Adafruit_MCP3008.h>
#include <Adafruit_NeoPixel.h>

#define LED_IN 2
#define LED_COUNT 4

#define CS1 13

#define D1 0
#define D2 1
#define D3 2
#define D4 3

#define HE1 0
#define HE2 1
#define HE3 2
#define HE4 3

int pixelFormat = NEO_GRB + NEO_KHZ800;

Adafruit_NeoPixel *pixels;
Adafruit_MCP3008 adc;

void setup() {
  pixels = new Adafruit_NeoPixel(LED_COUNT, LED_IN, pixelFormat);
  pixels->begin();
  
  adc.begin(SCK,MOSI,MISO,CS1);
}

void loop() {
  int HED1_val = adc.readADC(HE1);
  int HED2_val = adc.readADC(HE2);
  int HED3_val = adc.readADC(HE3);
  int HED4_val = adc.readADC(HE4);
  
  
  if(HED1_val < 930)
  {
    pixels->setPixelColor(D1, pixels->Color(0, 255, 0));
    pixels->show();
    }
  else{pixels->setPixelColor(D1, pixels->Color(255, 0, 0));pixels->show();}
  

  if(HED2_val < 930)
  {
    pixels->setPixelColor(D2, pixels->Color(0, 255, 0));
    pixels->show();
    }
  else{pixels->setPixelColor(D2, pixels->Color(255, 0, 0));pixels->show();}


  if(HED3_val < 930)
  {
    pixels->setPixelColor(D3, pixels->Color(0, 255, 0));
    pixels->show();
    }
  else{pixels->setPixelColor(D3, pixels->Color(255, 0, 0));pixels->show();}

  if(HED4_val < 930)
  {
    pixels->setPixelColor(D4, pixels->Color(0, 255, 0));
    pixels->show();
    }
  else{pixels->setPixelColor(D4, pixels->Color(255, 0, 0));pixels->show();}
}
