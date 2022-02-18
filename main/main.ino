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


  'buttons' or 'tiles' are used to describe led + sensor. therefore please do not get confused. They refer to the same component - Hall effect sensor.

*/

#define LED_IN 2
#define LED_COUNT 12

#define CS1 13
#define CS2 12
#define CS3 11

#define QUIT 10 // this is used by many other games and options,
                // thats why is equal to 10


// face - what face is being talked about?
// top/bottom  - is the button ,that is talked about, on top or bottom side?
// left/right - is the button ,that is talked about, on left or right side?
// this way, it is known which button is being selected e.g. to read sensor or set colour for led.
// especially for a case where only one led/sensor is being set/read.

//  naming convention: face   top/bottom   left/right
//            #define:  L         T            L
//              means: this sensor/led is located on left face on top-left corner.

//orentaion is being judged/determinated by the opposite face to the face with charging port.

#define LTL  0
#define LTR  1
#define LBL  2
#define LBR  3 //it always has to go _TL,_TR,_BL,_BR ; as this is how the leds are is wired up on each of the PCBs.
#define TTL  4
#define TTR  5
#define TBL  6
#define TBR  7
#define RTL  8
#define RTR  9
#define RBL  10
#define RBR  11


#define BOPEN 0
#define BCLOSED 1
#define BBOUNCE 2
#define BOUNCEDELAY 2 //wait specified number of cycles

typedef struct {
  int state; //state of the button
  int bCount; //bounce counter for a button
  int he_val; // read value from the hall effect sensor
  volatile bool ev; // signal for a button
  //int face;//on which face is the button located
  //note ; sensor is being treated like a regular button.
}button_t;

button_t buttons[12];

Adafruit_NeoPixel *leds;
Adafruit_MCP3008 left_face;
Adafruit_MCP3008 top_face;
Adafruit_MCP3008 right_face;

//LEFT FACE = b[0-3] --> buttons on the left face
//TOP FACE = b[4-7] --> buttons on the top face
//RIGHT FACE = b[8-11] --> buttons on the right face
// those are determinated by the connctions made with PCBs' leds (LED_in , LED_out).
// then that also relates to the hall effect sensors.

//set begin with GREEN and RED colour, this will be customisable via mobile app. note: there can be developed extra game modes.
uint32_t goodC = leds->Color(0, 32, 0);//colour which player gets points/correct hits.
uint32_t badC = leds->Color(32, 0, 0);//colour which player either lose the game or gets deducted points.

uint32_t onC = leds->Color(32, 32, 32);
uint32_t menuGameC = leds->Color(164, 0, 164);
uint32_t menuDebugC = leds->Color(168, 135, 50);
uint32_t menuAmbientC = leds->Color(121, 68, 0);

uint32_t game1 = leds->Color(32, 0, 0);

uint32_t red = leds->Color(32, 0, 0);
uint32_t orange = leds->Color(255, 140, 0);
uint32_t yellow = leds->Color(234, 255, 0);
uint32_t green = leds->Color(0, 255, 0);
uint32_t white = leds->Color(32, 32, 32);
uint32_t black = leds->Color(0, 0, 0);

//time stuff
/*****************************************************************/
unsigned long startMillis;
unsigned long currentMillis;      //need for 'timed' button press.
const unsigned long start_game_counter = 1000; // five sec
/*****************************************************************/

bool isPressed(int bvalue){
  if (bvalue < 900){return true;} // button has been pressed.
  else {return false;}// button has not been pressed.
  /*Note:
  Later can be upgraded and be able to determinate different strength of pressing.
  Probably good idea to create a struct that will hold the amount of strength
  applied.*/
}
void initButton(button_t *b){
    b->state = BOPEN;
    b->ev = false;
    b->bCount = 0;
    b->he_val = 0;
}
void SetnShow_1Led_SingleTypeColour(int LEDnum, uint32_t colour){
  leds->setPixelColor(LEDnum, colour);
  leds->show();
}
void SetnShow_1Led_HUE(int LEDnum, unsigned int hue, byte sat, byte val){
  leds->setPixelColor(LEDnum,leds->gamma32(leds->ColorHSV(hue, sat, val)));
  leds->show();
}
void SetnShow_1Led_RGB(int LEDnum,byte r, byte g, byte b){
  leds->setPixelColor(LEDnum, r,g,b);
  leds->show();
}

//there is a fill() function, however it does not allow for more complex logic e.g. with rainbow effect or other.
//therefore it is better to just make own
void SetnShow_AllLeds_SingleTypeColour(uint32_t colour){
  for(int i=0; i<LED_COUNT;i++){
    leds->setPixelColor(i, colour);
  }
  leds->show();
}
void SetnShow_AllLedsHUE(unsigned int hue, byte sat, byte val){
  for(int i=0; i<LED_COUNT;i++){
    leds->setPixelColor(i,leds->gamma32(leds->ColorHSV(hue, sat, val)));
  }
  leds->show();
}
void SetnShow_AllLedsRGB(byte r, byte g, byte b){
  for(int i=0; i<LED_COUNT;i++){
    leds->setPixelColor(i, r, g, b);
  }
  leds->show();
}
//sets all leds on with default 'on' colour (onC)
void ledsOn(){
  SetnShow_AllLeds_SingleTypeColour(onC);
}
void ledsOff(){
  leds->clear();
  leds->show();
}
void initButton_all(button_t b[]){
  for (int i = 0; i<12;i++){
    initButton(&b[i]);
  }

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
void buttonRead(button_t *b, int bOrder){
  if (bOrder>=0 && bOrder<=3){
    b->he_val = left_face.readADC(bOrder);
  }
  else if(bOrder>=4 && bOrder<=7) {
    b->he_val = top_face.readADC(bOrder);
  }
  else{
    b->he_val = right_face.readADC(bOrder);
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
int debugDelayLed = 150;
void AllComboLightsColours(){
  ledsOff();
  delay(debugDelayLed);
  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(64, 0, 0));
  }
  delay(debugDelayLed);

  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(0, 64, 0));
  }
  leds->show();
  delay(debugDelayLed);

  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(0, 64, 0));
  }
  leds->show();
  delay(debugDelayLed);

  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(0, 0, 64));
  }
  leds->show();
  delay(debugDelayLed);
  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(64, 64, 0));
  }
  leds->show();
  delay(debugDelayLed);

  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(0, 64, 64));
  }
  leds->show();
  delay(debugDelayLed);

  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(64, 0, 64));
  }
  leds->show();
  delay(debugDelayLed);

  for(int i = 0; i<12; i++){
    leds->setPixelColor(i, leds->Color(64, 64, 64));
  }
  leds->show();
  delay(debugDelayLed);

  //old code idea...
  /*leds->show();
  startMillis = millis();
  currentMillis = millis();
  while (currentMillis - startMillis <= debugDelayLed){
    buttonRead_all(b);
    debug_HEvalues(b);
    currentMillis = millis();
  }

  if (isButtonSelected(&buttons[RBR],TTL,black)){
    break;
  }*/

}
//sets colour of the menu option - always goes before conditional statment with isButtonSelected().
void ButtonSelectColour(int bOrder,uint32_t colour){
  SetnShow_1Led_SingleTypeColour(bOrder,colour);
}
//any button that is hold for 1 second, it will start the game sequence. (This hold only is true only when the cube is started)
//reads a specific button and if user presses&holds specific button for 1sec the function returns true, if not then false.
// this function is for selection of menu options on the cube such as games menu, debug menu and any other menus to come or game options.
bool isButtonSelected(button_t *b, int bOrder){
  buttonRead(b,bOrder);
  if(b->ev){
    startMillis = millis();// this will record the time when entered the start of the timer
    while(b->ev){// need to 'trap' the current button that is pressed
      debug_HEvalues(buttons);
      buttonRead(b,bOrder);// to update if the user stopped pressing the button.
      currentMillis=millis();// this will help calculate if one second has been passed.
      if(currentMillis - startMillis >= start_game_counter){return true;}// and if the user hold it for long enough, start the game sequence.
    }
  }
  else{startMillis = 0;currentMillis=0;}// if a user stops holding it - reset the values and proceed to the next button to check.
  return false;
}
void displayScore(int points){
  leds->clear();
  leds->show();
  for(int i = 0; i<points;i++){
    if (i<=3){
      SetnShow_1Led_SingleTypeColour(i,red);
      delay(750);
      }
    else if((i>=4) && (i<=7)){
      SetnShow_1Led_SingleTypeColour(0,orange);SetnShow_1Led_SingleTypeColour(1,orange);SetnShow_1Led_SingleTypeColour(2,orange);SetnShow_1Led_SingleTypeColour(3,orange);
      SetnShow_1Led_SingleTypeColour(i,orange);
      delay(750);
    }
    else
    {
      SetnShow_1Led_SingleTypeColour(0,green);SetnShow_1Led_SingleTypeColour(1,green);SetnShow_1Led_SingleTypeColour(2,green);SetnShow_1Led_SingleTypeColour(3,green);
      SetnShow_1Led_SingleTypeColour(4,green);SetnShow_1Led_SingleTypeColour(5,green);SetnShow_1Led_SingleTypeColour(6,green);SetnShow_1Led_SingleTypeColour(7,green);
      SetnShow_1Led_SingleTypeColour(i,green);
      delay(750);
    }
  }
  delay(750);
}
void selectedOptionIndicatorFlashes(){
  ledsOff();
  delay(250);
  SetnShow_AllLeds_SingleTypeColour(white);
  delay(250);
  ledsOff();
  delay(250);
  SetnShow_AllLeds_SingleTypeColour(white);
  delay(250);
  ledsOff();
  delay(250);
  SetnShow_AllLeds_SingleTypeColour(white);
  delay(250);
  ledsOff();
  delay(250);
}
void CountDownFlashes(){
  ledsOff();
  delay(500);
  SetnShow_AllLeds_SingleTypeColour(white);
  delay(1000);
  ledsOff();
  delay(1000);
  for (int i = 4; i<12;i++){
    SetnShow_1Led_SingleTypeColour(i,white);
  }
  delay(1000);
  ledsOff();
  delay(1000);
  for (int i = 4; i<8;i++){
    SetnShow_1Led_SingleTypeColour(i,white);
  }
  delay(1000);
  ledsOff();
  delay(1000);
}



#define COUNTDOWN 0
#define setMoleSquirrel 1
#define hitOrNotHit 2
#define dispScore 3
int game_state = COUNTDOWN;
int mole=0, squirrel=0, prev_mole=0, prev_squirrel=0;
int player_points;//points are earned when user pressed a button with goodC colour (the mole), and will recive no point for taking too long or hit the badC coloure button (the squarel).
int game_level = 0;
unsigned long game_counter = 5000;// not making it constant as this could be varied for difficulty of the game purposes
const unsigned long delayforbutton = 1000;
unsigned long current2Millis;

//games logic functions
void mole_squirrel_game(button_t b[]){
  while(1){
    debug_HEvalues(buttons);
    switch(game_state){
      case COUNTDOWN:
        CountDownFlashes();
        game_state = setMoleSquirrel;
        break;

      case setMoleSquirrel:
        buttonRead_all(b);
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
        SetnShow_1Led_SingleTypeColour(mole, goodC);
        SetnShow_1Led_SingleTypeColour(squirrel, badC);
        game_state = hitOrNotHit;
        }

        break;

      case hitOrNotHit:
        startMillis = millis();
        while (1){
          currentMillis = millis();
          buttonRead_all(b);
          if (b[mole].ev){
            player_points++;
            ledsOff();
            current2Millis = millis();
            while(current2Millis - currentMillis <= delayforbutton){current2Millis = millis();}// instead of blocking delay().
            game_state = setMoleSquirrel;
            break;
          }
          else if (b[squirrel].ev || (currentMillis - startMillis >= game_counter)){
            ledsOff();
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
    if (game_state == QUIT){
      break;
    }
  }
}
void restartGameStats(){
  game_state = COUNTDOWN;
  game_level = 0;
  player_points = 0;

}
//Template for creating a game:
/*void name_of_the_game(button_t b[]){
  while(1){
    switch(_game_state_of_the_game){
      //game logic here//
    }
    if (game_state == QUIT){
      break;
    }
  }
}*/


//TODO:not working properly - does not work when us isButtonSelected() applied
// somehow the sensor's value (that we want to read to go back to option menu) is reading -1..
// reason: still unknown.
// without the isButtonSelected() exit routine, it does work as intendted however there is no way to go back to the
// main menu.

#define LED 0
#define HE_READING 1
int debug_state = LED;
void debug(button_t b[]){
  while(1){
    switch(debug_state){
      case LED:
        for (int i = 0; i<5; i++){
          AllComboLightsColours();
        }
        debug_state = HE_READING;
        break;
      case HE_READING:
        buttonRead_all(b);
        debug_HEvalues(b);
        for (int i = 0; i<12;i++){
          if(b[i].ev)
          {
            SetnShow_1Led_SingleTypeColour(i, goodC);
            }
          else{SetnShow_1Led_SingleTypeColour(i,badC);}
        }
        break;
      }
   }

}


//TODO:same problem as debug - -1 on the button that meant to work as a button to leave infinite loop.

/*Effect functions - for ambient effects to place your cube on the desk and just admire the colours*/
int tile,previousTile;
int oneEffHUE,previousOneEffHUE;
byte oneEffSAT,previousOneEffSAT; // 0-256
void OneFadeInOutTile_Effect(button_t b[]){
  for(int count = 0; count<20;count++){
    previousTile = tile;
    tile = random(0, 12);
    while(tile==previousTile){tile = random(0, 12);}

    previousOneEffHUE = oneEffHUE;
    oneEffHUE = random(0, 65536);
    while(oneEffHUE==previousOneEffHUE){oneEffHUE = random(0, 65536);}

    previousOneEffSAT = oneEffSAT;
    oneEffSAT = random(0, 256);
    while(oneEffSAT==previousOneEffSAT){oneEffSAT = random(128, 256);}

    for(int i = 0; i<256; i++){
      SetnShow_1Led_HUE(tile, oneEffHUE, oneEffSAT, i);
      delay(10);
    }

    for(int i = 255; i>=0; i--){
      SetnShow_1Led_HUE(tile, oneEffHUE, oneEffSAT, i);
      delay(10);
    }
  }
}








void setup() {
  //needed setup intrustions for leds and sensors.
  leds = new Adafruit_NeoPixel(LED_COUNT, LED_IN, NEO_GRB + NEO_KHZ800);
  leds->begin();
  left_face.begin(SCK,MOSI,MISO,CS1);
  top_face.begin(SCK,MOSI,MISO,CS2);
  right_face.begin(SCK,MOSI,MISO,CS3);

  randomSeed(analogRead(0));// for random selection of tiles to be lit. Used for game purposes purely.
  Serial.begin(9600);// for debug purposes, for now using serial monitor.
  initButton_all(buttons);
  ledsOn();

}
void loop() {
  debug_HEvalues(buttons);
  /*Games Menu*/
  ButtonSelectColour(TTL,menuGameC);//colour of thr games menu tile
  if (isButtonSelected(&buttons[TTL],TTL)){//Game Menu selected by press&hold top left button on top face.
     selectedOptionIndicatorFlashes();
     //different colour lights are on, and have different game modes
     ledsOn();//to make non-game-option tiles with onColour
     while(1){
      ButtonSelectColour(TTL,game1);
      /*Game 1*/ //whac_a_mole_not_squirrel
      if (isButtonSelected(&buttons[TTL],TTL)){
         mole_squirrel_game(buttons);
         if (game_state == QUIT){
           ledsOn();
           restartGameStats();
           break;
         }
       }
      /*Game #*/
      /*GAME-INCLUDE TEMPLATE HERE*/

      //add more else-if statments for more games to select on the cube- preferably most frequent games but the closest to the beginning,
      //of the if-else statment there.
      // Up to 12 games to select. either manually coding it 'or using the WhacACubeAPP' - (future development)


    }

  }
  /*Debug Menu*/
  ButtonSelectColour(TTR,menuDebugC);//colour of thr games menu tile
  if (isButtonSelected(&buttons[TTR],TTR)){//i would put else-if but because of the buttonSelectColour() it has to be if.
    selectedOptionIndicatorFlashes();
    debug(buttons);
    }
  /*'Cube saver' Menu*/ //has nice visuals where random colour and random tile is selected
  ButtonSelectColour(TBL,menuAmbientC);
  if (isButtonSelected(&buttons[TBL],TBL)){
   selectedOptionIndicatorFlashes();
   while(1){
    OneFadeInOutTile_Effect(buttons);
    break;
   }
  }
  //else{}//do nothing
}




// GAME_INCLUDE TEMPLATE:
// Template for including the game in the Game Menu:
// /*Game #*/ else if (isButtonSelected(&buttons[_orientation_Macro_For_Button],_orientation_Macro_For_Button, _Colour_To_lit_the button){
//    /*Include the game here*/
//    if (game_state == QUIT){
//      ledsOn();
//      restartGameStats();
//      break;
//    }
// }
// also delete the '//' at the beginning.
