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


//the sensors value can have from 0 - 1024(10 bit binary representation of voltage), this sensor has base value of around 950 (2.78V)
// when magnet with North face gets close to the HE sensor it increases value being read. For Soth face is the reverse.
// the logic adopted is for South face - greater range of values. This will be usefull for future further development for more complex applciations/games.

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
/*******************************************************************************
 * 
 * this funtion introduces a method of reading a button press knows as polling. 
 * 
 * if the sensor's value is below specificed threshold, this would suggest that 
 * magnet has been pressed down towards the sensor. 
 * 
********************************************************************************/
void pollButtonTask(button_t *b){
  if (b->bCount > 0) b->bCount -- ;
  switch (b->state)
  {
    case BOPEN:// user has not pressed button yet
        if (b->he_val<930) {// value chnaged below 930 
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
            b->ev = false;
        }
        break;
  }
    return;
}

void debug_HEvalues(button_t b[]){
  Serial.println("");
  Serial.print("D1:");Serial.println(b[0].he_val);
  Serial.print("D2:");Serial.println(b[1].he_val);
  Serial.print("D3:");Serial.println(b[2].he_val);
  Serial.print("D4:");Serial.println(b[3].he_val);
  Serial.print("D5:");Serial.println(b[4].he_val);
  Serial.print("D6:");Serial.println(b[5].he_val);
  Serial.print("D7:");Serial.println(b[6].he_val);
  Serial.print("D8:");Serial.println(b[7].he_val);
  Serial.print("D9:");Serial.println(b[8].he_val);
  Serial.print("D10:");Serial.println(b[9].he_val);
  Serial.print("D11:");Serial.println(b[10].he_val);
  Serial.print("D12:");Serial.println(b[11].he_val);
}

void setup() {
  pixels = new Adafruit_NeoPixel(LED_COUNT, LED_IN, pixelFormat);
  pixels->begin();

  Serial.begin(9600);
  Serial.println("MCP3008 simple test.");

  left_adc.begin(SCK,MOSI,MISO,CS1);
  top_adc.begin(SCK,MOSI,MISO,CS2);
  right_adc.begin(SCK,MOSI,MISO,CS3);

  

}

void loop() {

  for (int i = 0; i<12;i++){
    initPollButton(&b[i]);
  }
  while(1){
    for (int i = 0; i<4;i++){
      b[i].he_val = left_adc.readADC(i);
      b[i+4].he_val = top_adc.readADC(i);
      b[i+8].he_val = right_adc.readADC(i);
    }
    
    debug_HEvalues(b);
    
    for (int i = 0; i<12;i++){
      pollButtonTask(&b[i]);
    }
    
    for (int i = 0; i<12;i++){
      if(b[i].ev)
      {
         // wihtout this it will always stay true...
        pixels->setPixelColor(i, pixels->Color(0, 64, 0));
        pixels->show();
        }
      else{pixels->setPixelColor(i, pixels->Color(64, 0, 0));pixels->show();}
    }
  }
}
