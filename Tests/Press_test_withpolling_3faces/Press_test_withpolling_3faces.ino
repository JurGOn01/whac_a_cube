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
Adafruit_MCP3008 left_adc;
Adafruit_MCP3008 top_adc;
Adafruit_MCP3008 right_adc;

struct button_t{
  int state; //state of the button
  int bCount; //bounce counter for a button
  int he_val; // read value from the hall effect sensor
  volatile bool ev; // signal for a button
  //note ; sensor is being treated like a regular button.
};
button_t b[12];

void initPollButton(button_t *b){
    b->state = BOPEN ;
    b->ev = false ;
    b->bCount = 0 ;
}

void pollButtonTask(button_t *b){
  if (b->bCount > 0) b->bCount -- ;
  switch (b->state)
  {
    case BOPEN:
        if (b->he_val<930) {
            b->state = BCLOSED ;
            b->ev = true ;
        }
        break ;

    case BCLOSED:
        if (!(b->he_val < 930)) {
            b->state = BBOUNCE ;
            b->bCount = BOUNCEDELAY ;
        }
        break;

    case BBOUNCE:
        if (b->he_val < 930) {
            b->state = BCLOSED ;
        }
        else if (b->bCount == 0) {
            b->state = BOPEN ;
        }
        break;
  }
    return;
}

void setup() {
  pixels = new Adafruit_NeoPixel(LED_COUNT, LED_IN, pixelFormat);
  pixels->begin();

  left_adc.begin(SCK,MOSI,MISO,CS1);
  top_adc.begin(SCK,MOSI,MISO,CS2);
  right_adc.begin(SCK,MOSI,MISO,CS3);

}

void loop() {

  for (int i = 0; i<12;i++){
    initPollButton(&b[i]);
  }
  for (int i = 0; i<4;i++){
    b[i].he_val = left_adc.readADC(i);
    b[i+4].he_val = top_adc.readADC(i);
    b[i+8].he_val = right_adc.readADC(i);
  }
  for (int i = 0; i<12;i++){
    pollButtonTask(&b[i]);
  }
  for (int i = 0; i<12;i++){
    if(b[i].ev)
    {
      pixels->setPixelColor(i, pixels->Color(0, 64, 0));
      pixels->show();
      }
    else{pixels->setPixelColor(i, pixels->Color(64, 0, 0));pixels->show();}
  }
}
