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

#define BOPEN 0
#define BCLOSED 1
#define BBOUNCE 2
#define BOUNCEDELAY 2


int pixelFormat = NEO_GRB + NEO_KHZ800;

Adafruit_NeoPixel *pixels;
Adafruit_MCP3008 adc;

struct button_t{
  int state; //state of the button
  int bCount; //bounce counter for a button
  int he_val; // read value from the hall effect sensor
  volatile bool ev; // signal for a button
  //note ; sensor is being treated like a regular button.
};
button_t b[4];

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

  adc.begin(SCK,MOSI,MISO,CS1);
}

void loop() {

  for (int i = 0; i<4;i++){
    initPollButton(&b[i]);
  }
  for (int i = 0; i<4;i++){
    b[i].he_val = adc.readADC(i);
  }
  for (int i = 0; i<4;i++){
    pollButtonTask(&b[i]);
  }
  for (int i = 0; i<4;i++){
    if(b[i].ev)
    {
      pixels->setPixelColor(i, pixels->Color(0, 64, 0));
      pixels->show();
      }
    else{pixels->setPixelColor(i, pixels->Color(64, 0, 0));pixels->show();}
  }
}
