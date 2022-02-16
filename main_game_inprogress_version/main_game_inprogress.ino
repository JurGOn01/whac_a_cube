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

#define CS1 13
#define CS2 12
#define CS3 11

#define BOPEN 0
#define BCLOSED 1
#define BBOUNCE 2
#define BOUNCEDELAY 2 //wait specified number of cycles

typedef struct {
  int state; //state of the button
  int bCount; //bounce counter for a button
  int he_val; // read value from the hall effect sensor
  volatile bool ev; // signal for a button
  //note ; sensor is being treated like a regular button.
}button_t;

Adafruit_NeoPixel *leds;
Adafruit_MCP3008 left_face;
Adafruit_MCP3008 top_face;
Adafruit_MCP3008 right_face;

button_t buttons[12];
//LEFT FACE = b[0-3] --> buttons on the left face
//TOP FACE = b[4-7] --> buttons on the top face
//RIGHT FACE = b[8-11] --> buttons on the right face

//set begin with GREEN and RED colour, this will be customisable via mobile app. note: there can be developed extra game modes.
uint32_t goodC = leds->Color(0, 64, 0);//colour which player gets points/correct hits.
uint32_t badC = leds->Color(64, 0, 0);//colour which player either lose the game or gets deducted points.

uint32_t red = leds->Color(64, 0, 0);
uint32_t orange = leds->Color(255, 140, 0);
uint32_t yellow = leds->Color(234, 255, 0);
uint32_t green = leds->Color(0, 64, 0);
uint32_t white = leds->Color(64, 64, 64);

bool isPressed(int bvalue){
  if (bvalue < 900){return true;} // button has been pressed.
  else {return false;}// button has not been pressed.
  /*Note:
  Later can be upgraded and be able to determinate different strength of pressing.
  Probably good idea to create a struct that will hold the amount of strength
  applied.*/
}
void initButton(button_t *b){
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
void setColour(int LEDnum, uint32_t colour){
  leds->setPixelColor(LEDnum, colour);
  leds->show();
}
void initButton_all(button_t b[]){
  for (int i = 0; i<12;i++){
    initButton(&b[i]);
  }
}

void pollButtonTask(button_t *b){
  if (b->bCount > 0) b->bCount -- ;
  switch (b->state)
  {
    case BOPEN:
        if (isPressed(b->he_val)) {
            b->state = BCLOSED ;
            b->ev = true ;
        }
        break ;

    case BCLOSED:
        if (!isPressed(b->he_val)) {
            b->state = BBOUNCE ;
            b->bCount = BOUNCEDELAY ;
        }
        break;

    case BBOUNCE:
        if (isPressed(b->he_val)) {
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

void buttonRead(button_t *b,int order){
  if (order>=0 && order<=3){
    b[order].he_val = left_face.readADC(order);
  }
  else if(order>=4 && order<=7) {
    b[order].he_val = top_face.readADC(order);
  }
  else{
    b[order].he_val = right_face.readADC(order);
  }//if more faces are added later just more conditional statements

  pollButtonTask(b);
}
void buttonRead_all(button_t b[]){
  for (int i = 0; i<4;i++){
    b[i].he_val = left_face.readADC(i);
    b[i+4].he_val = top_face.readADC(i);
    b[i+8].he_val = right_face.readADC(i);
  }

  for (int i = 0; i<12;i++){
    pollButtonTask(&b[i]);
  }
}

void debug_HEvalues(button_t b[]){
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
void debug_white(){
  setColour(0,leds->Color(64, 64, 64));
}
void debug_red(){
  setColour(0,red);
}
void allLEDsOn_White(){
  for(int i = 0; i<12;i++){
    setColour(i,white);
  }
}
/*****************************************************************/
unsigned long startMillis;
unsigned long currentMillis;      //need for 'timed' button press.
const unsigned long start_game_counter = 1000; // five sec
/*****************************************************************/
//any button that is hold for 1 second, it will start the game sequence. (This hold only is true only when the cube is started)
bool isGameOn(button_t b[]){
  readingAllSensors(b);
  pollAllButtons(b);
  for (int i = 0; i<12;i++){
    if(b[i].ev){
      startMillis = millis();// this will record the time when entered the start of the timer
      while(b[i].ev){// need to 'trap' the current button that is pressed
        debug_HEvalues(buttons);
        buttonRead(&b[i],i)// to update if the user stopped pressing the button.
        currentMillis=millis();// this will help calculate if one second has been passed.
        if(currentMillis - startMillis >= start_game_counter){return true;}// and if the user hold it for long enough, start the game sequence.
      }
    }
    else{startMillis = 0;currentMillis=0;}// if a user stops holding it-reset the values, do nothing and proceed to the next button to check.
  }
  return false;
}

#define COUNTDOWN 0
#define setMoleSquirrel 1
#define hitOrNotHit 2
#define dispScore 3
#define QUIT 4


void displayScore(int points){
  leds->clear();
  leds->show();
  for(int i = 0; i<points;i++){
    if (i<=3){
      setColour(i,red);
      delay(750);
      }
    else if((i>=4) && (i<=7)){
      setColour(0,orange);setColour(1,orange);setColour(2,orange);setColour(3,orange);
      setColour(i,orange);
      delay(750);
    }
    else
    {
      setColour(0,green);setColour(1,green);setColour(2,green);setColour(3,green);
      setColour(4,green);setColour(5,green);setColour(6,green);setColour(7,green);
      setColour(i,green);
      delay(750);
    }
  }
}
void selectedOptionIndicatorFlashes(){
  leds->clear();
  leds->show();
  delay(500);
  for (int i = 0; i<12;i++){
    setColour(i,leds->Color(64, 64, 64));
  }
  delay(500);
  leds->clear();
  leds->show();
  delay(500);
  for (int i = 0; i<12;i++){
    setColour(i,leds->Color(64, 64, 64));
  }
  delay(500);
  leds->clear();
  leds->show();
  delay(500);
  for (int i = 0; i<12;i++){
    setColour(i,leds->Color(64, 64, 64));
  }
  delay(500);
  leds->clear();
  leds->show();
  delay(500);
}
void CountDownFlashes(){

  for (int i = 0; i<12;i++){
    setColour(i,leds->Color(64, 64, 64));
  }
  delay(1000);
  leds->clear();
  leds->show();
  delay(1000);
  for (int i = 4; i<12;i++){
    setColour(i,leds->Color(64, 64, 64));
  }
  delay(1000);
  leds->clear();
  leds->show();
  delay(1000);
  for (int i = 4; i<8;i++){
    setColour(i,leds->Color(64, 64, 64));
  }
  delay(1000);
  leds->clear();
  leds->show();
  delay(1000);
}

int game_state = COUNTDOWN;
int mole=0, squirrel=0, prev_mole=0, prev_squirrel=0;
int player_points;//points are earned when user pressed a button with goodC colour (the mole), and will recive no point for taking too long or hit the badC coloure button (the squarel).
int game_level = 0;
unsigned long game_counter = 5000;// not making it constant as this could be varied for difficulty of the game purposes
const unsigned long delayforbutton = 1000;
unsigned long current2Millis;

void game(button_t b[]){
  switch(game_state){
    case COUNTDOWN:
      CountDownFlashes();
      game_state = setMoleSquirrel;
      break;

    case setMoleSquirrel:
      //readingAllSensors(b);
      //pollAllButtons(b);
      prev_mole = mole;
      prev_squirrel = squirrel;
      mole = random(0, 12);
      squirrel = random(0, 12);

      while(mole == prev_mole || mole == prev_squirrel || mole == squirrel){
        mole = random(0, 12);
      }
      while(squirrel == prev_mole || squirrel == prev_squirrel || squirrel == mole){
        squirrel = random(0, 12);
      }

      game_level++;
      if (game_level > 12){game_state = dispScore;}//this prevented the short show up of goodC and badC before displayingScore()
      else{
      setColour(mole, goodC);
      setColour(squirrel, badC);
      game_state = hitOrNotHit;
      }

      break;

    case hitOrNotHit:
      startMillis = millis();
      while (1){
        currentMillis = millis();
        readingAllSensors(b);
        pollAllButtons(b);
        if (b[mole].ev){
          player_points++;
          leds->clear();
          leds->show();
          current2Millis = millis();
          while(current2Millis - currentMillis <= delayforbutton){current2Millis = millis();}// instead of blocking delay().
          game_state = setMoleSquirrel;
          break;
        }
        else if (b[squirrel].ev || (currentMillis - startMillis >= game_counter)){
          leds->clear();
          leds->show();
          current2Millis = millis();
          while(current2Millis - currentMillis <= delayforbutton){current2Millis = millis();}// instead of blocking delay().
          game_state = setMoleSquirrel;
          break;
        }
      }
      break;

    case dispScore:
      displayScore(player_points);
      game_state = QUIT;
      break;
  }
}
void restartGameStats(){
  game_state = COUNTDOWN;
  game_level = 0;
  player_points = 0;

}


void setup() {
  leds = new Adafruit_NeoPixel(LED_COUNT, LED_IN, NEO_GRB + NEO_KHZ800);
  leds->begin();

  left_face.begin(SCK,MOSI,MISO,CS1);
  top_face.begin(SCK,MOSI,MISO,CS2);
  right_face.begin(SCK,MOSI,MISO,CS3);

  Serial.begin(9600);

  randomSeed(analogRead(0));
}

void loop() {
  initButton_all(buttons);
  debug_HEvalues(buttons);
  allLEDsOn_White();
  if (isGameOn(buttons)){
    selectedOptionIndicatorFlashes();
    while(1){
      debug_HEvalues(buttons);
      game(buttons);
      if (game_state == QUIT){
        leds->clear();
        leds->show();
        restartGameStats();
        break;
      }
    }
  }
}
