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
        if (b->he_val < 900) {// value chnaged below 930
            b->state = BCLOSED ;
            b->ev = true ;
        }
        break ;

    case BCLOSED:
        if (!(b->he_val < 900)) {
            b->state = BBOUNCE ;
            b->bCount = BOUNCEDELAY ;
        }
        break;

    case BBOUNCE:
        if (b->he_val < 900) {
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
uint32_t goodC = pixels->Color(0, 255, 0);//colour which player gets points/correct hits.
uint32_t badC = pixels->Color(255, 0, 0);

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



uint32_t pressureGoodC;

uint32_t isSensitivePress(int bvalue){
// version 1: any colour value can be used, however only 5 levels of brigthness change
//  if (bvalue <= 900 && bvalue > 800){pressureGoodC = goodC * 0.2;}//20
//  else if (bvalue <= 800 && bvalue > 700){pressureGoodC = goodC * 0.4;}//40%
//  else if (bvalue <= 700 && bvalue > 600){pressureGoodC = goodC * 0.6;}//60%
//  else if (bvalue <= 600 && bvalue > 500){pressureGoodC = goodC * 0.8;}//80%
//  else if (bvalue <= 500){pressureGoodC = goodC;}

//version 2:
// if (bvalue <= 900 && bvalue > 880){pressureGoodC = pixels->Color(0, (125/16), (255/16));}
// else if (bvalue <= 880 && bvalue > 860){pressureGoodC = pixels->Color(0, (125/15), (255/15));}
// else if (bvalue <= 860 && bvalue > 840){pressureGoodC = pixels->Color(0, (125/14), (255/14));}
// else if (bvalue <= 840 && bvalue > 820){pressureGoodC = pixels->Color(0, (125/13), (255/13));}
// else if (bvalue <= 820 && bvalue > 800){pressureGoodC = pixels->Color(0, (125/12), (255/12));}
// else if (bvalue <= 800 && bvalue > 780){pressureGoodC = pixels->Color(0, (125/11), (255/11));}
// else if (bvalue <= 780 && bvalue > 760){pressureGoodC = pixels->Color(0, (125/10), (255/10));}
// else if (bvalue <= 760 && bvalue > 740){pressureGoodC = pixels->Color(0, (125/9), (255/9));}
// else if (bvalue <= 740 && bvalue > 720){pressureGoodC = pixels->Color(0, (125/8), (255/8));}
// else if (bvalue <= 720 && bvalue > 700){pressureGoodC = pixels->Color(0, (125/7), (255/7));}
// else if (bvalue <= 700 && bvalue > 680){pressureGoodC = pixels->Color(0, (125/6), (255/6));}
// else if (bvalue <= 680 && bvalue > 660){pressureGoodC = pixels->Color(0, (125/5), (255/5));}
// else if (bvalue <= 660 && bvalue > 640){pressureGoodC = pixels->Color(0, (125/4), (255/4));}
// else if (bvalue <= 640 && bvalue > 620){pressureGoodC = pixels->Color(0, (125/3), (255/3));}
// else if (bvalue <= 620 && bvalue > 600){pressureGoodC = pixels->Color(0, (125/2), (255/2));}
// else if (bvalue <= 600){pressureGoodC = pixels->Color(0, (125/1), (255/1));}

//version 3:
// if (bvalue <= 900 && bvalue > 880){pressureGoodC = goodC/16;}
// else if (bvalue <= 880 && bvalue > 860){pressureGoodC = goodC/15;}
// else if (bvalue <= 860 && bvalue > 840){pressureGoodC = goodC/14;}
// else if (bvalue <= 840 && bvalue > 820){pressureGoodC = goodC/13;}
// else if (bvalue <= 820 && bvalue > 800){pressureGoodC = goodC/12;}
// else if (bvalue <= 800 && bvalue > 780){pressureGoodC = goodC/11;}
// else if (bvalue <= 780 && bvalue > 760){pressureGoodC = goodC/10;}
// else if (bvalue <= 760 && bvalue > 740){pressureGoodC = goodC/9;}
// else if (bvalue <= 740 && bvalue > 720){pressureGoodC = goodC/8;}
// else if (bvalue <= 720 && bvalue > 700){pressureGoodC = goodC/7;}
// else if (bvalue <= 700 && bvalue > 680){pressureGoodC = goodC/6;}
// else if (bvalue <= 680 && bvalue > 660){pressureGoodC = goodC/5;}
// else if (bvalue <= 660 && bvalue > 640){pressureGoodC = goodC/4;}
// else if (bvalue <= 640 && bvalue > 620){pressureGoodC = goodC/3;}
// else if (bvalue <= 620 && bvalue > 600){pressureGoodC = goodC/2;}
// else if (bvalue <= 600){pressureGoodC = goodC/1;}

  return pressureGoodC;
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
        pixels->setPixelColor(i, isSensitivePress(b[i].he_val));
        pixels->show();
        
        }
      else{pixels->setPixelColor(i, badC);pixels->show();}
    }
  }
}
