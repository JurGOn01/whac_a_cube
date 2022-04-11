/********************************************************************************************
*Title:Press_test_with_polling_3faces
*Description:
*Extension of Press_test_with_polling, where three faces are being tested at the same time.
*All three faces are going to lit with red colour initially, when south face magnet is near
*any of the sensors on any of the faces(PCBs) - it will light up with green colour.
*
*Also here a debug function has been made to read the real-time values from each of the sensors.
*
*Author: Jerzy Aleksander Gorak
*********************************************************************************************/

#include <Adafruit_MCP3008.h>
#include <Adafruit_NeoPixel.h>

#define LED_IN 2
#define LED_COUNT 12

#define CS1 13
#define CS2 12
#define CS3 11

#define D1 0
#define D2 1
#define D3 2
#define D4 3

#define HE1 0
#define HE2 1
#define HE3 2
#define HE4 3

#define BOPEN 0
#define BCLOSED 1
#define BBOUNCE 2
#define BOUNCEDELAY 2


int pixelFormat = NEO_GRB + NEO_KHZ800;

Adafruit_NeoPixel *pixels;

uint32_t goodC = pixels->Color(0, 255, 0);//colour which player gets points/correct hits.
uint32_t badC = pixels->Color(255, 0, 0);

void setup() {
  pixels = new Adafruit_NeoPixel(LED_COUNT, LED_IN, pixelFormat);
  pixels->begin();
  Serial.begin(9600);
}

void loop() {
  for(int i = 1; i<=15;i++){
    Serial.println(pixels->Color(0, (125/(16-i)), (17*i)));
  }
}
