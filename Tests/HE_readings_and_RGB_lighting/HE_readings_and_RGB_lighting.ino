/*****************************************************************************
Title: HES_testing

Description:
Simple program to read the analouge values of the Hall effect sensors on the 
PCB. This is done by accessing the mcp3004 on the pcb and selecing 
corresponding channel for the desired sensor. 


Author: Jerzy Aleksander Gorak
******************************************************************************/
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MCP3008.h>

#define LED_IN 2
#define LED_COUNT 4
#define CS1 13 // CS1 because there are more than one mcp3004 to deal with.(1 per PCB aka face) 

#define D1 0
#define D2 1
#define D3 2
#define D4 3
#define D5 4

Adafruit_MCP3008 adc;
Adafruit_NeoPixel *pixels;



void rgb()
{
    pixels->setPixelColor(D1, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D2, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D3, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D4, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D5, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D6, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D7, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D8, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D9, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D10, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D11, pixels->Color(255, 0, 0));
    pixels->setPixelColor(D12, pixels->Color(255, 0, 0));
    
    pixels->show();
    delay(150);
    pixels->setPixelColor(D1, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D2, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D3, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D4, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D5, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D6, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D7, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D8, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D9, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D10, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D11, pixels->Color(0, 255, 0));
    pixels->setPixelColor(D12, pixels->Color(0, 255, 0));
    pixels->show();
    delay(150);
    pixels->setPixelColor(D1, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D2, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D3, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D4, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D5, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D6, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D7, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D8, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D9, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D10, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D11, pixels->Color(0, 0, 255));
    pixels->setPixelColor(D12, pixels->Color(0, 0, 255));
    pixels->show();
    delay(150);
    
}
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("MCP3008 simple test.");

  adc.begin(SCK,MOSI,MISO,CS1);

  pixels = new Adafruit_NeoPixel(LED_COUNT, LED_IN, NEO_GRB + NEO_KHZ800);
  
  pixels->begin(); // initialises NeoPixel strip object 

  //         (sck, mosi, miso, cs);
  //adc.begin(13,    11,   12, 10);
}

void loop() {
  
  Serial.print(adc.readADC(0)); Serial.print("\t");
  Serial.print("["); Serial.print("D1"); Serial.println("]");
  Serial.print(adc.readADC(1)); Serial.print("\t");
  Serial.print("["); Serial.print("D2"); Serial.println("]");
  Serial.print(adc.readADC(2)); Serial.print("\t");
  Serial.print("["); Serial.print("D3"); Serial.println("]");
  Serial.print(adc.readADC(3)); Serial.print("\t");
  Serial.print("["); Serial.print("D4"); Serial.println("]");
  
  rgb();
}
