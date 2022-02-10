/*
Title:
Description:

Author: Jerzy Aleksander Gorak
 */

#include <Adafruit_NeoPixel.h>
#include <Adafruit_MCP3008.h>
#include <stdbool.h>
#include <stdint.h>


/*
  Documentation
  =============


  'buttons' and 'sensors' are used interchangly. therefore please do not get confused. They refer to the same component - Hall effect sensor.
*/

#define LED_IN 2
#define LED_COUNT 12

#define CS_LEFT 13
#define CS_TOP 12
#define CS3_RIGHT 11

//LEDs' (D) index numbers
#define D1 0
#define D2 1
#define D3 2
#define D4 3
//Hall effect sensors' (HE) channel numbers
#define HE1 0
#define HE2 1
#define HE3 2
#define HE4 3

#define BOPEN 0
#define BCLOSED 1
#define BBOUNCE 2
#define BOUNCEDELAY 2 //wait specified number of cycles

struct button_t{
  int state; //state of the button
  int bCount; //bounce counter for a button
  int HE_value; // read value from the hall effect sensor
  volatile bool ev; // signal for a button
  //note ; sensor is being treated like a regular button.
};

Adafruit_NeoPixel *leds;
Adafruit_MCP3008 leftFaceHEs;
Adafruit_MCP3008 topFaceHEs;
Adafruit_MCP3008 rightFaceHEs;

button_t b[12];
//LEFT FACE = b[0-3] --> buttons on the left face
//TOP FACE = b[0-3] --> buttons on the top face
//RIGHT FACE = b[0-3] --> buttons on the right face

//set begin with GREEN and RED colour, this will be customisable via mobile app. note: there can be developed extra game modes.
uint32_t goodC = leds->Color(0, 255, 0);//colour which player gets points/correct hits.
uint32_t badC = leds->Color(255, 0, 0);//colour which player either lose the game or gets deducted points.


//void serialDebug_HE();
void buttonTestFlashing();

void setup() {
  leds = new Adafruit_NeoPixel(LED_COUNT, LED_IN, NEO_GRB + NEO_KHZ800);
  leds->begin();

  leftFaceHEs.begin(SCK,MOSI,MISO,CS_LEFT);

  initPollAllButtons();

  Serial.begin(9600);
  while (!Serial);
}

void loop() {
  readingAllSensors();
  pollAllButtons();
  //serialDebug_HE();
  buttonTestFlashing();
}

bool isPressed(int bvalue){
  if (bvalue < 930){return true;} // button has been pressed.
  else {return false;}// button has not been pressed.
  /*Note:
  Later can be upgraded and be able to determinate different strength of pressing.
  Probably good idea to create a struct that will hold the amount of strength
  applied.*/
}

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
        if (isPressed(b->HE_value)) {
            b->state = BCLOSED ;
            b->ev = true ;
        }
        break ;

    case BCLOSED:
        if (!isPressed(b->HE_value)) {
            b->state = BBOUNCE ;
            b->bCount = BOUNCEDELAY ;
        }
        break;

    case BBOUNCE:
        if (isPressed(b->HE_value)) {
            b->state = BCLOSED ;
        }
        else if (b->bCount == 0) {
            b->state = BOPEN ;
        }
        break;
  }
    return;
}


void setColour(int LEDnum, uint32_t colour){
  leds->setPixelColor(LEDnum, colour);
  leds->show();
}

void readingAllSensors(){
  b1.HE_value = leftFaceHEs.readADC(HE1);
  b2.HE_value = leftFaceHEs.readADC(HE2);
  b3.HE_value = leftFaceHEs.readADC(HE3);
  b4.HE_value = leftFaceHEs.readADC(HE4);
  b5.HE_value = topFaceHEs.readADC(HE1);
  b6.HE_value = topFaceHEs.readADC(HE2);
  b7.HE_value = topFaceHEs.readADC(HE3);
  b8.HE_value = topFaceHEs.readADC(HE4);
  b9.HE_value = rightFaceHEs.readADC(HE1);
  b10.HE_value = rightFaceHEs.readADC(HE2);
  b11.HE_value = rightFaceHEs.readADC(HE3);
  b12.HE_value = rightFaceHEs.readADC(HE4);
}

void pollAllButtons(){
  pollButtonTask(&b1);
  pollButtonTask(&b2);
  pollButtonTask(&b3);
  pollButtonTask(&b4);
  pollButtonTask(&b5);
  pollButtonTask(&b6);
  pollButtonTask(&b7);
  pollButtonTask(&b8);
  pollButtonTask(&b9);
  pollButtonTask(&b10);
  pollButtonTask(&b11);
  pollButtonTask(&b12);
}

void initPollAllButtons(){
  initPollButton(&b1);
  initPollButton(&b2);
  initPollButton(&b3);
  initPollButton(&b4);
  initPollButton(&b5);
  initPollButton(&b6);
  initPollButton(&b7);
  initPollButton(&b8);
  initPollButton(&b9);
  initPollButton(&b10);
  initPollButton(&b11);
  initPollButton(&b12);
}

void buttonTestFlashing(){
  if(b1.ev==true)
  {
    setColour(D1, goodC);
    }
  else{setColour(D1, badC);}


  if(b2.ev==true)
  {
    setColour(D2, goodC);
    }
  else{setColour(D2, badC);}


  if(b3.ev==true)
  {
    setColour(D3, goodC);
    }
  else{setColour(D3, badC);}

  if(b4.ev==true)
  {
    setColour(D4, goodC);
    }
  else{setColour(D4, badC);}
}























/*
Bunch of code that is an alternative way of doing same thing
*/

/*struct rgb_colour{
  int r;
  int g;
  int b;
};

//rgb_colour goodC = {.g = 255};
//rgb_colour badC = {.r = 255};
*/
