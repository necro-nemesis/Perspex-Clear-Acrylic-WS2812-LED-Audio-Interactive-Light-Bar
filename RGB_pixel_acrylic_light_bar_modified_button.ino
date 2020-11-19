//************
//Sound Level to light show sketch for the
//autogain microphone Amplifier from Adafruit on pin AO
//plus neopixel led functions (pin 6) mapped on to different sound levels to give music to light effects
//
//*************

//lines below set variables for neopixels

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define NUMBER_PIXEL 15
#define LEDPIN    6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_PIXEL, LEDPIN, NEO_GRB + NEO_KHZ800);

//Global Variables

//Timing Block

int lng = 70;//long delay
int mod = 50;//moderate delay
int sht = 25;//short delay
int hyper = 10;//very short delay
int extreme = 5;//exceedingly short delay

//Misc Variables

uint8_t eepromAddress = 130; //Location to store last mode state during power cycle.
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
byte toggleIdle = 1;//Level=0 toggle between idle routines.
int toggleCount = 0;
int pushButton = 2;// Assign pushbutton to input 2.
int modeSelect = 0;// Button routine selection.
boolean buttonState = 0;//
unsigned int spectrumColor[3];
unsigned int pattern [5][3];//Pixel pattern Array 5 rows, 3 RGB values.
boolean UsingBar = false; //Used by Cylon function. Set this to true If you are using the 8x1 Neopixel Bar Or you want to only use 3 leds for the scanner.
unsigned int soundLevel;
unsigned int pixelLength = 1;
unsigned int loudEffect;
unsigned int sound;


//Function Prototypes

void CylonEyeUp(uint32_t Co, uint32_t Ct, uint32_t Ctt, uint8_t Delay, int TotalPixels, int pStart);
void CylonEyeDown(uint32_t Co, uint32_t Ct, uint32_t Ctt, uint8_t Delay, int TotalPixels, int pEnd);
void Seizure(unsigned int flashRate);
void rainbowCycle(uint8_t wait, boolean direction);
void pixelFill(int pixelLength);
void turnBlack();
void colorWipe(uint32_t c, uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void Sparkle(byte red, byte green, byte blue, int SpeedDelay);
int Sample();
void setColor();


void setup()
{
  strip.begin();//initialises neopixels
  strip.setBrightness(255);// set brightness from 0 to max is 255
  strip.show();//clears any previous data in the strip
  pinMode (6, OUTPUT);
  pinMode (pushButton,INPUT);
  Serial.begin(9600);//set up for checking mic is working
}

void loop()
{
  // Select Mode of operation
  modeSelect = EEPROM.read(eepromAddress);
  buttonState = digitalRead(pushButton);
  if (modeSelect>3)
    {
      modeSelect=0;
      EEPROM.write(eepromAddress,modeSelect);
      for (int i = 0; i < 5; i++) {
        CylonEyeUp(strip.Color(175, 0, 0), strip.Color(25, 0, 0), strip.Color(10, 0, 0), hyper, strip.numPixels(), 0);
        //delay(hyper);
        CylonEyeDown(strip.Color(175, 0, 0), strip.Color(25, 0, 0), strip.Color(10, 0, 0), hyper, strip.numPixels(), 0);
        //delay(hyper);
      }//close Cylon;
    }
      else if (buttonState==1)
    {
      modeSelect++;
      EEPROM.write(eepromAddress,modeSelect);
      Seizure(hyper);  
    }
  Serial.print("  mode= ");
  Serial.println(modeSelect);
  switch (modeSelect){
    case 0:
   
  //Mode 0 Audio Light Show.
  //lines below are for the microphone sampling from Adafruit autogain mic
  //open void loop
  //first run the sound sampling




 
  soundLevel=Sample();

  
  //next section is a series of 12 (0-11) 'if' statements which trigger different patterns.
  //it is a combination of a traditional VU style meter fill of the strip
  // combined with randomised animated patterns to keep it interesting
  if (soundLevel > 0)
  {
   toggleIdle = ! toggleIdle;
   toggleCount = 0;
  }
  if (soundLevel == 0)
  {
    //open if 0. When there is silence a rainbow pattern runs
    if (toggleIdle == 0 && toggleCount < 3)
    {
      rainbowCycle(1,1);//from the neopixel library
      toggleCount++;
    }
    if (toggleIdle == 0 && toggleCount >=3)
    {
      rainbowCycle(4,1);
    }
    if (toggleIdle == 1)
    {
      for (int i = 0; i < 1; i++) {
        int x = random(0, 256); int y = random(0, 256); int z = random(0, 256);
        CylonEyeUp(strip.Color(x, y, z), strip.Color(x / 2, y / 2, z / 2), strip.Color(x / 4, y / 4, z / 4), sht, strip.numPixels(), 0);
        CylonEyeDown(strip.Color(x, y, z), strip.Color(x / 2, y / 2, z / 2), strip.Color(x / 4, y / 4, z / 4), sht, strip.numPixels(), 0);
        toggleIdle = ! toggleIdle;
      }
    }
  }//close if 0 statement

  if (soundLevel == 1)
  {
    //open level 1 if statement which contains 5 randomised options

    int level1Color = random(1, 7);//choose random number 1 - 5
    pixelLength = 2;

    if (level1Color == 1) //if random no 1 choses show
    {
      pattern[0][0] = 0; pattern[0][1] = 255; pattern[0][1] = 255;
      pixelFill(pixelLength);
    }//close random no. 1

    else if (level1Color == 2) //if random no 2 choses show green
    {
      pattern[0][0] = 0; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close green

    else if (level1Color == 3) //run blue
    {
      pattern[0][0] = 0; pattern[0][1] = 0; pattern[0][1] = 255;
      pixelFill(pixelLength);
    }//close blue

    else if (level1Color == 4) //run yellow
    {
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close yellow

    else if (level1Color == 5) //run Cylon red
    { //Cylon loop
      for (int i = 0; i < 1; i++) {
        CylonEyeUp(strip.Color(175, 0, 0), strip.Color(25, 0, 0), strip.Color(10, 0, 0), sht, strip.numPixels(), 0);
        delay(sht);
        CylonEyeDown(strip.Color(175, 0, 0), strip.Color(25, 0, 0), strip.Color(10, 0, 0), sht, strip.numPixels(), 0);
        delay(sht);
        CylonEyeUp(strip.Color(random(0, 256), random(0, 256), random(0, 256)), strip.Color(0, 0, 0), strip.Color(0, 0, 0), hyper, strip.numPixels(), 0);
      }//close Cylon
    }//close red

    else if (level1Color == 6) //turn black
    {
      turnBlack();
    }
  }//end of if sound level 1 options


  if (soundLevel == 2)
  {
    //open level 2

    int level2Color = random(1, 7);//choose one of 5 options if sound level 2
    pixelLength = 3;
    if (level2Color == 1) //run red mix
    {

      strip.setPixelColor(0, 255, 0, 0);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 255, 0, 0);
      strip.setPixelColor(3, 0, 0, 0);
      strip.setPixelColor(4, 0, 0, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level2Color == 2) //run green mix
    {
      //open option 2
      pattern[0][0] = 0; pattern[0][1] = 206; pattern[0][1] = 209;
      pixelFill(pixelLength);
    }//close green

    else if (level2Color == 3) //run blue mix
    {
      //open option 3

      strip.setPixelColor(0, 0, 0, 255);
      strip.setPixelColor(1, 255, 0, 0);
      strip.setPixelColor(2, 0, 0, 255);
      strip.setPixelColor(3, 0, 0, 0);
      strip.setPixelColor(4, 0, 0, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level2Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close yellow

    else if (level2Color == 5)//for a bit of variation 1 in 5 of level 2 will show a pattern across whole strip:
      //open if 5 Cylon Yellow
    { //Cylon loop
      for (int i = 0; i < 2; i++) {
        CylonEyeUp(strip.Color(175, 175, 0), strip.Color(25, 25, 0), strip.Color(10, 10, 0), hyper, strip.numPixels(), strip.numPixels() / 2);
        delay(sht);
        CylonEyeDown(strip.Color(175, 175, 0), strip.Color(25, 25, 0), strip.Color(10, 10, 0), hyper, strip.numPixels(), strip.numPixels() / 2);
        delay(sht);
      }//close Cylon
    }//close of option 5

    else if (level2Color == 6) //turn black
    {
      turnBlack();
    }//close option 6
  }//close level 2

  if (soundLevel == 3)
  {
    //open if sound level 3
    int level3Color = random(1, 7);
    pixelLength = 4;
    if (level3Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);
      strip.setPixelColor(1, 0, 255, 0);
      strip.setPixelColor(2, 255, 0, 0);
      strip.setPixelColor(3, 0, 255, 0);
      strip.setPixelColor(4, 0, 0, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level3Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 245, 116, 97);
      strip.setPixelColor(1, 169, 221, 20);
      strip.setPixelColor(2, 245, 116, 97);
      strip.setPixelColor(3, 169, 221, 20);
      strip.setPixelColor(4, 0, 0, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 2green

    else if (level3Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 169, 221, 199);
      strip.setPixelColor(1, 245, 116, 97);
      strip.setPixelColor(2, 169, 221, 199);
      strip.setPixelColor(3,  245, 116, 97);
      strip.setPixelColor(4, 0, 0, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level3Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 4 yellow

    else if (level3Color == 5)
    {
      //open option 5
      strip.setPixelColor(0, 255, 255, 255);
      strip.setPixelColor(1, 255, 105, 180);
      strip.setPixelColor(2, 255, 255, 255);
      strip.setPixelColor(3, 255, 105, 180);
      strip.setPixelColor(4, 255, 255, 255);
      strip.setPixelColor(5, 255, 105, 180);
      strip.setPixelColor(6, 255, 255, 255);
      strip.setPixelColor(7, 255, 105, 180);
      strip.setPixelColor(8, 255, 255, 255);
      strip.setPixelColor(9, 255, 105, 180);
      strip.setPixelColor(10, 255, 255, 255);
      strip.setPixelColor(11, 255, 105, 180);
      strip.setPixelColor(12, 255, 255, 255);
      strip.setPixelColor(13, 255, 105, 180);
      strip.setPixelColor(14, 255, 255, 255);
      strip.show();
      delay(sht);
    }//close of option 5
    else if (level3Color == 6) //turn black
    {
      turnBlack();
    }//close option 6
  }//close level 3


  if (soundLevel == 4)
  {
    //open if sound level 4
    int level4Color = random(1, 7);
    pixelLength = 5;
    if (level4Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 255, 0, 0);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 255, 0, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close red

    else if (level4Color == 2) //run green
    {
      //open option 2
      pattern[0][0] = 0; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close green

    else if (level4Color == 3) //run blue
    {
      //open option 3
      pattern[0][0] = 0; pattern[0][1] = 0; pattern[0][1] = 255;
      pixelFill(pixelLength);
    }//close blue

    else if (level4Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close yellow

    else if (level4Color == 5)
    {
      //Cylon loop
      for (int i = 0; i < 2; i++) {
        CylonEyeUp(strip.Color(0, 175, 175), strip.Color(0, 25, 25), strip.Color(0, 25, 25), hyper, strip.numPixels(), 0);
        delay(sht);
        CylonEyeDown(strip.Color(175, 0, 175), strip.Color(25, 0, 25), strip.Color(10, 0, 10), hyper, strip.numPixels(), 0);
        delay(20);
      }//close Cylon
    }//close option 5

  }//close if sound level 4

  else if (soundLevel == 5)
  {
    //open if sound level 5


    int level5Color = random(1, 7);
    pixelLength = 6;
    if (level5Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);
      strip.setPixelColor(1, 255, 255, 255);
      strip.setPixelColor(2, 0, 0, 255);
      strip.setPixelColor(3, 255, 0, 0);
      strip.setPixelColor(4, 255, 255, 255);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 1 red

    else if (level5Color == 2) //run green
    {
      //open option 2
      pattern[0][0] = 0; pattern[0][1] = 255; pattern[0][1] = 255;
      pixelFill(pixelLength);
    }//close option 2 green

    else if (level5Color == 3) //run blue
    {
      //open option 3
      pattern[0][0] = 0; pattern[0][1] = 0; pattern[0][1] = 255;
      pixelFill(pixelLength);
    }//close option 3 blue

    else if (level5Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close yellow

    else if (level5Color == 5)
    {
      //open option 5

      strip.setPixelColor(0, 255, 0, 0);
      strip.setPixelColor(1, 0, 255, 0);
      strip.setPixelColor(2, 0, 0, 255);
      strip.setPixelColor(3, 255, 0, 0);
      strip.setPixelColor(4, 0, 255, 0);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 0, 255);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(7, 0, 255, 0);
      strip.setPixelColor(8, 0, 0, 255);
      strip.setPixelColor(9, 255, 0, 0);
      strip.setPixelColor(10, 0, 255, 0);
      strip.setPixelColor(11, 0, 0, 255);
      strip.setPixelColor(12, 255, 0, 0);
      strip.setPixelColor(13, 0, 255, 0);
      strip.setPixelColor(14, 0, 0, 255);
      strip.show();
      delay(lng);
    }//close option 5

    else if (level5Color == 6)
    {
      //open option 6

      colorWipe(strip.Color(255, 0, 255), sht); // magenta
      colorWipe(strip.Color(0, 255, 0), sht); // green
      strip.show();
    }//close option 6
  }//close if sound level 5


  else if (soundLevel == 6)
  {
    //open if soundlevel 6

    int level6Color = random(1, 7);
    pixelLength = 7;
    if (level6Color == 1) //run red
    {
      //open option 1

      strip.setPixelColor(0, 255, 0, 0);
      strip.setPixelColor(1, 255, 255, 255);
      strip.setPixelColor(2, 0, 0, 255);
      strip.setPixelColor(3, 255, 0, 0);
      strip.setPixelColor(4, 255, 255, 255);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 1red

    else if (level6Color == 2) //run green
    {
      //open option 2
      pattern[0][0] = 0; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 2 green

    else if (level6Color == 3) //run blue
    {
      //open option 3
      pattern[0][0] = 0; pattern[0][1] = 0; pattern[0][1] = 255;
      pixelFill(pixelLength);
    }//close option 3 blue

    else if (level6Color == 4) //run yellow
    {
      //open option 4

      strip.setPixelColor(0, 148, 0, 211);
      strip.setPixelColor(1, 75, 0, 130);
      strip.setPixelColor(2, 0, 0, 255);
      strip.setPixelColor(3, 0, 255, 0);
      strip.setPixelColor(4, 255, 255, 0);
      strip.setPixelColor(5, 255, 127, 0);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(sht);
    }//close yellow

    else if (level6Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(0, 0, 255), sht); // Blue
      colorWipe(strip.Color(255, 255, 0), sht); // yellow
      strip.show();
    }//close option 5

    else if (level6Color == 6)
    {
      //open option6

      theaterChase(strip.Color(200, 0, 0), 60); // Red
      strip.show();
      delay(lng);
    }//close option 6
  }//close if sound level 6

  else if (soundLevel == 7)
  {
    //open if sound level 7
    int level7Color = random(1, 8);
    pixelLength = 8;
    if (level7Color == 1) //run red
    {
      //open option 1
      pattern[0][0] = 255; pattern[0][1] = 0; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 1 red

    else if (level7Color == 2) //run green
    {
      //open option 2
      pattern[0][0] = 0; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 2 green

    else if (level7Color == 3) //run blue
    {
      //open option 3
      pattern[0][0] = 0; pattern[0][1] = 0; pattern[0][1] = 255;
      pixelFill(pixelLength);
    }//close option 3 blue

    else if (level7Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 4 yellow

    else if (level7Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(255, 20, 147), sht); // pink
      colorWipe(strip.Color(0, 206, 209), sht); // turquoise
      strip.show();
      delay(lng);
    }//close option 5

    else if (level7Color == 6)
    {
      //open option 6

      theaterChase(strip.Color(255, 20, 100), 60); // Red
      strip.show();
      delay(sht);
    }//close option 6

    else if (level7Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);
      strip.setPixelColor(1, 0, 100, 0);
      strip.setPixelColor(2, 255, 0, 70);
      strip.setPixelColor(3, 50, 0, 150);
      strip.setPixelColor(4, 0, 70, 70);
      strip.setPixelColor(5, 0, 100, 0);
      strip.setPixelColor(6, 255, 0, 70);
      strip.setPixelColor(7, 50, 0, 150);
      strip.setPixelColor(0, 0, 70, 70);
      strip.setPixelColor(8, 255, 0, 70);
      strip.setPixelColor(9, 0, 100, 0);
      strip.setPixelColor(10, 255, 0, 70);
      strip.setPixelColor(11, 50, 0, 150);
      strip.setPixelColor(12, 0, 70, 70);
      strip.setPixelColor(13, 0, 100, 0);
      strip.setPixelColor(14, 255, 0, 70);

      strip.show();
      delay(sht);
    }//close option 7
  }//close if sound level 7

  else if (soundLevel == 8)
  {
    //open if sound level 8

    int level8Color = random(1, 8);

    if (level8Color == 1) //run red
    {
      //open option 1
      pattern[0][0] = 255; pattern[0][1] = 0; pattern[0][1] = 0;
      pixelFill(pixelLength);

    }//close option 1 red

    else if (level8Color == 2) //run green/blue
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 0, 255, 0);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 0, 255, 0);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 255, 0);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 0, 255, 0);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 2 green/blue

    else if (level8Color == 3) //run turquoise / blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 206, 255);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 0, 206, 255);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 0, 206, 255);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 206, 255);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 0, 206, 255);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level8Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 4 yellow

    else if (level8Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(255, 20, 147), sht); // pink
      colorWipe(strip.Color(0, 206, 209), sht); // turquoise
      strip.show();
    }//close option 5

    else if (level8Color == 6)
    {
      //open option 6

      theaterChase(strip.Color(0, 206, 209), 60); // Red
      strip.show();
      delay(sht);
    }//close option 6

    else if (level8Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);
      strip.setPixelColor(1, 0, 100, 0);
      strip.setPixelColor(2, 255, 0, 70);
      strip.setPixelColor(3, 50, 0, 150);
      strip.setPixelColor(4, 0, 70, 70);
      strip.setPixelColor(5, 0, 100, 0);
      strip.setPixelColor(6, 255, 0, 70);
      strip.setPixelColor(7, 50, 0, 150);
      strip.setPixelColor(0, 0, 70, 70);
      strip.setPixelColor(8, 255, 0, 70);
      strip.setPixelColor(9, 0, 100, 0);
      strip.setPixelColor(10, 255, 0, 70);
      strip.setPixelColor(11, 50, 0, 150);
      strip.setPixelColor(12, 0, 70, 70);
      strip.setPixelColor(13, 0, 100, 0);
      strip.setPixelColor(14, 255, 0, 70);

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);
      strip.setPixelColor(1, 0, 255, 255);
      strip.setPixelColor(2, 0, 0, 0);
      strip.setPixelColor(3, 255, 255, 0);
      strip.setPixelColor(4, 255, 255, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 255, 255);
      strip.setPixelColor(7, 0, 255, 255);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 255, 255, 0);
      strip.setPixelColor(10, 255, 255, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 255, 255);
      strip.setPixelColor(13, 0, 255, 255);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);


    }//close option 7

  }//close if sound level 8

  else if (soundLevel == 9)
  {
    //open if sound level 9

    int level9Color = random(1, 9);

    if (level9Color == 1) //run red
    {
      //open option 1
      pattern[0][0] = 255; pattern[0][1] = 0; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 1 red

    else if (level9Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 0, 255, 0);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 0, 255, 0);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 255, 0);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 0, 255, 0);
      strip.setPixelColor(9, 0, 0, 255);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 2 green

    else if (level9Color == 3) //run blue
    {
      //open option 3
      strip.setPixelColor(0, 255, 0, 255);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 255, 0, 255);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 255, 0, 255);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 255, 0, 255);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 255, 0, 255);
      strip.setPixelColor(9, 0, 0, 255);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
    }//close option 3blue

    else if (level9Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 4 yellow

    else if (level9Color == 5)
    {
      //open option 5
      colorWipe(strip.Color(255, 255, 255), sht); // white
      colorWipe(strip.Color(0, 206, 209), sht); // turquoise
      strip.show();
    }//close option 5

    else if (level9Color == 6)
    {
      //open option 6

      theaterChase(strip.Color(50, 190, 209), 60); // turquise
      strip.show();
      delay(lng);
    }//close option 6

    else if (level9Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);
      strip.setPixelColor(1, 0, 100, 0);
      strip.setPixelColor(2, 255, 0, 70);
      strip.setPixelColor(3, 50, 0, 150);
      strip.setPixelColor(4, 0, 70, 70);
      strip.setPixelColor(5, 0, 100, 0);
      strip.setPixelColor(6, 255, 0, 70);
      strip.setPixelColor(7, 50, 0, 150);
      strip.setPixelColor(8, 255, 0, 70);
      strip.setPixelColor(9, 0, 100, 0);
      strip.setPixelColor(10, 255, 0, 70);
      strip.setPixelColor(11, 50, 0, 150);
      strip.setPixelColor(12, 0, 70, 70);
      strip.setPixelColor(13, 0, 100, 0);
      strip.setPixelColor(14, 255, 0, 70);

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);
      strip.setPixelColor(1, 0, 255, 255);
      strip.setPixelColor(2, 0, 0, 0);
      strip.setPixelColor(3, 255, 255, 0);
      strip.setPixelColor(4, 255, 255, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 255, 255);
      strip.setPixelColor(7, 0, 255, 255);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 255, 255, 0);
      strip.setPixelColor(10, 255, 255, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 255, 255);
      strip.setPixelColor(13, 0, 255, 255);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);

      strip.setPixelColor(0, 255, 50, 50);
      strip.setPixelColor(1, 0, 255, 0);
      strip.setPixelColor(2, 255, 50, 50);
      strip.setPixelColor(3, 255, 0, 0);
      strip.setPixelColor(4, 255, 50, 50);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 255, 50, 50);
      strip.setPixelColor(7, 0, 255, 0);
      strip.setPixelColor(8, 255, 50, 50);
      strip.setPixelColor(9, 255, 0, 0);
      strip.setPixelColor(10, 255, 50, 50);
      strip.setPixelColor(11, 0, 0, 255);
      strip.setPixelColor(12, 255, 50, 50);
      strip.setPixelColor(13, 0, 255, 0);
      strip.setPixelColor(14, 255, 50, 50);
      strip.show();

      delay(lng);


    }//close option 7

    else if (level9Color == 8)
    {
      //open option 8
      strip.setPixelColor(0, 255, 255, 255);
      strip.setPixelColor(1, 0, 0, 0);
      strip.setPixelColor(2, 255, 255, 255);
      strip.setPixelColor(3, 0, 0, 0);
      strip.setPixelColor(4, 255, 255, 255);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 255, 255, 255);
      strip.setPixelColor(7, 0, 0, 0);
      strip.setPixelColor(8, 255, 255, 255);
      strip.setPixelColor(9, 0, 0, 0);
      strip.setPixelColor(10, 255, 255, 255);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 255, 255, 255);
      strip.setPixelColor(13, 0, 0, 0);
      strip.setPixelColor(14, 255, 255, 255);

      strip.show();
      delay (lng);

      strip.setPixelColor(0, 0, 0, 0);
      strip.setPixelColor(1, 255, 255, 255);
      strip.setPixelColor(2, 0, 0, 0);
      strip.setPixelColor(3, 255, 255, 255);
      strip.setPixelColor(4, 0, 0, 0);
      strip.setPixelColor(5, 255, 255, 255);
      strip.setPixelColor(6, 0, 0, 0);
      strip.setPixelColor(7, 255, 255, 255);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 255, 255, 255);
      strip.setPixelColor(10, 0, 0, 0);
      strip.setPixelColor(11, 255, 255, 255);
      strip.setPixelColor(12, 0, 0, 0);
      strip.setPixelColor(13, 255, 255, 255);
      strip.setPixelColor(14, 0, 0, 0);

      strip.show();
      delay(lng);


    }//close option 8

  }//close if sound level 9

  else if (soundLevel == 10)

  {
    //open if sound Level 10

    int level10Color = random(1, 9);

    if (level10Color == 1) //run red
    {
      //open option 1
      pattern[0][0] = 255; pattern[0][1] = 0; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 1 red

    else if (level10Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 0, 255, 0);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 0, 255, 0);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 255, 0);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 0, 255, 0);
      strip.setPixelColor(9, 0, 0, 255);
      strip.setPixelColor(10, 0, 255, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 255, 0);
      strip.setPixelColor(13, 0, 0, 255);
      strip.setPixelColor(14, 0, 255, 0);

      strip.show();
      delay(lng);
    }//close option 2 green

    else if (level10Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 206, 255);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 0, 206, 255);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 0, 206, 255);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 206, 255);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 0, 206, 255);
      strip.setPixelColor(9, 0, 0, 255);
      strip.setPixelColor(10, 0, 206, 255);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 206, 255);
      strip.setPixelColor(13, 0, 0, 255);
      strip.setPixelColor(14, 0, 206, 255);
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level10Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 4 yellow

    else if (level10Color == 5)
    {
      //open option 5

      colorWipe(strip.Color(200, 40, 147), sht); // pink
      colorWipe(strip.Color(0, 206, 209), sht); // turquoise
      strip.show();
      delay(sht);
    }//close option 5

    else if (level10Color == 6)
    {
      //open option 6
      theaterChase(strip.Color(0, 206, 209), 60);
      strip.show();
      delay(sht);
    }//close option 6

    else if (level10Color == 7)
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);
      strip.setPixelColor(1, 0, 100, 0);
      strip.setPixelColor(2, 255, 0, 70);
      strip.setPixelColor(3, 50, 0, 150);
      strip.setPixelColor(4, 0, 70, 70);
      strip.setPixelColor(5, 0, 100, 0);
      strip.setPixelColor(6, 255, 0, 70);
      strip.setPixelColor(7, 50, 0, 150);
      strip.setPixelColor(8, 255, 0, 70);
      strip.setPixelColor(9, 0, 100, 0);
      strip.setPixelColor(10, 255, 0, 70);
      strip.setPixelColor(11, 50, 0, 150);
      strip.setPixelColor(12, 0, 70, 70);
      strip.setPixelColor(13, 0, 100, 0);
      strip.setPixelColor(14, 255, 0, 70);

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);
      strip.setPixelColor(1, 0, 255, 255);
      strip.setPixelColor(2, 0, 0, 0);
      strip.setPixelColor(3, 255, 255, 0);
      strip.setPixelColor(4, 255, 255, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 255, 255);
      strip.setPixelColor(7, 0, 255, 255);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 255, 255, 0);
      strip.setPixelColor(10, 255, 255, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 255, 255);
      strip.setPixelColor(13, 0, 255, 255);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 255;
      pixelLength = strip.numPixels();
      pixelFill(pixelLength);
      delay(sht);
      }
      else if (level10Color == 8)
      {
        Seizure(hyper);
    }//close option 8

  }//close if sound level 10


  else if (soundLevel == 11)

  {
    //open if sound Level 11

    loudEffect = random (1,5);
    if (loudEffect==1){
    for (int i = 0; i < 12; i++)
    {
      Sparkle(255, 255, 255, 80);
    }
    }//close loudEffect Sparkle

    else if (loudEffect==2){
    CylonEyeUp(strip.Color(255, 255, 255), strip.Color(125,125,125), strip.Color(65,65,65), hyper, strip.numPixels(), 0);
    }//close loudEffect CylonEyeUp
    else if (loudEffect==3){
    CylonEyeDown(strip.Color(255, 255, 255), strip.Color(125,125,125), strip.Color(65,65,65), hyper, strip.numPixels(), 0);
    }//close loudEffect CylonEyeDown
    else {
      turnBlack(); 
    }//close loudEffect Black  
   
    int level11Color = random(1, 8);
    pixelLength = strip.numPixels();
    if (level11Color == 1) //run red
    {
      //open option 1
      pattern[0][0] = 255; pattern[0][1] = 0; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 1 red

    else if (level11Color == 2) //run green
    {
      //open option 2

      strip.setPixelColor(0, 0, 255, 0);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 0, 255, 0);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 0, 255, 0);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 255, 0);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 0, 255, 0);
      strip.setPixelColor(9, 0, 0, 255);
      strip.setPixelColor(10, 0, 255, 0);
      strip.setPixelColor(11, 0, 0, 255);
      strip.setPixelColor(12, 0, 255, 0);
      strip.setPixelColor(13, 0, 0, 255);
      strip.setPixelColor(14, 0, 255, 0);
      strip.show();
      delay(lng);
    }//close option 2  green

    else if (level11Color == 3) //run blue
    {
      //open option 3

      strip.setPixelColor(0, 0, 206, 255);
      strip.setPixelColor(1, 0, 0, 255);
      strip.setPixelColor(2, 0, 206, 255);
      strip.setPixelColor(3, 0, 0, 255);
      strip.setPixelColor(4, 0, 206, 255);
      strip.setPixelColor(5, 0, 0, 255);
      strip.setPixelColor(6, 0, 206, 255);
      strip.setPixelColor(7, 0, 0, 255);
      strip.setPixelColor(8, 0, 206, 255);
      strip.setPixelColor(9, 0, 0, 255);
      strip.setPixelColor(10, 0, 206, 255);
      strip.setPixelColor(11, 0, 0, 255);
      strip.setPixelColor(12, 0, 206, 255);
      strip.setPixelColor(13, 0, 0, 255);
      strip.setPixelColor(14, 0, 206, 255);
      strip.show();
      delay(lng);
    }//close option 3 blue

    else if (level11Color == 4) //run yellow
    {
      //open option 4
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 0;
      pixelFill(pixelLength);
    }//close option 4 yellow

    else if (level11Color == 5)
    {
      //open option 5

      colorWipe(strip.Color(0, 40, 255), sht); // pink
      colorWipe(strip.Color(0, 209, 206), sht); // turquoise
      strip.show();
      delay(sht);
    }//close option 5

    else if (level11Color == 6) //open option 6
    {
      //open option 6
      theaterChase(strip.Color(0, 206, 109), 60);
      strip.show();
      delay(sht);
    }//close option 6

    else if (level11Color == 7)//open option 7
    {
      //open option 7
      strip.setPixelColor(0, 0, 70, 70);
      strip.setPixelColor(1, 0, 100, 0);
      strip.setPixelColor(2, 255, 0, 70);
      strip.setPixelColor(3, 50, 0, 150);
      strip.setPixelColor(4, 0, 70, 70);
      strip.setPixelColor(5, 0, 100, 0);
      strip.setPixelColor(6, 255, 0, 70);
      strip.setPixelColor(7, 50, 0, 150);
      strip.setPixelColor(8, 255, 0, 70);
      strip.setPixelColor(9, 0, 100, 0);
      strip.setPixelColor(10, 255, 0, 70);
      strip.setPixelColor(11, 50, 0, 150);
      strip.setPixelColor(12, 0, 70, 70);
      strip.setPixelColor(13, 0, 100, 0);
      strip.setPixelColor(14, 255, 0, 70);

      strip.show();
      delay(lng);

      strip.setPixelColor(0, 0, 255, 255);
      strip.setPixelColor(1, 0, 255, 255);
      strip.setPixelColor(2, 0, 0, 0);
      strip.setPixelColor(3, 255, 255, 0);
      strip.setPixelColor(4, 255, 255, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 255, 255);
      strip.setPixelColor(7, 0, 255, 255);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 255, 255, 0);
      strip.setPixelColor(10, 255, 255, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 255, 255);
      strip.setPixelColor(13, 0, 255, 255);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);
      pattern[0][0] = 255; pattern[0][1] = 255; pattern[0][1] = 255;
      pixelFill(pixelLength);

      strip.setPixelColor(0, 0, 255, 255);
      strip.setPixelColor(1, 0, 255, 255);
      strip.setPixelColor(2, 0, 0, 0);
      strip.setPixelColor(3, 255, 255, 0);
      strip.setPixelColor(4, 255, 255, 0);
      strip.setPixelColor(5, 0, 0, 0);
      strip.setPixelColor(6, 0, 255, 255);
      strip.setPixelColor(7, 0, 255, 255);
      strip.setPixelColor(8, 0, 0, 0);
      strip.setPixelColor(9, 255, 255, 0);
      strip.setPixelColor(10, 255, 255, 0);
      strip.setPixelColor(11, 0, 0, 0);
      strip.setPixelColor(12, 0, 255, 255);
      strip.setPixelColor(13, 0, 255, 255);
      strip.setPixelColor(14, 0, 0, 0);
      strip.show();
      delay(lng);

    }//close option 7

  }//close if sound level 11

break;
  case 1:
    setColor();
break;

  case 2:
    rainbowCycle(20,1);
break;
    
  case 3:  
  int direction=1;
  int rate = 0;
  int frequency;
  for (rate=0; rate<11; rate++){ 
    for (frequency=(40-rate)/5; frequency>1; frequency--) {
      rainbowCycle(rate,direction);
     }    
  }        
  direction=0;
  delay(1000);
  for (rate=10; rate>-1; rate--){
    for (frequency=(40-rate)/5; frequency>1; frequency--) {
      rainbowCycle(rate,direction);
     }
  }

  }//close switch case
}//close void loop()


//neopixel functions below



void colorWipe(uint32_t c, uint8_t wait)
{
  //open colorwipe
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}//close colorWipe function

void theaterChase(uint32_t c, uint8_t wait)
{
  //open theaterchase function
  for (int j = 0; j < 4; j++) //do 4 cycles of chasing
  {
    for (int q = 0; q < 3; q++)
    {
      for (int i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}//close theater chase function

void rainbowCycle(uint8_t wait, boolean direction)  //open rainbow function
{
  uint16_t i, j;

  if (direction==1){

  for (j = 0; j < 256; j++) // 1 cycles of all colors on wheel
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    buttonState = digitalRead(pushButton);
    if (buttonState==1) {
      break;
      }
    delay(wait);
  }
  }
  else{
  
  for (j = 256; j > 0; j--) // 1 cycles of all colors on wheel
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    buttonState = digitalRead(pushButton);
    if (buttonState==1) {
      break;
      }
    delay(wait);
  }  
  }
}//close rainbow function


//you need to included the code below for the neopixel functions that use the 'wheel' code:
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


// Sparkle Function

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(strip.numPixels());
  strip.setPixelColor(Pixel, red, green, blue);
  strip.show();
  delay(SpeedDelay);
  strip.setPixelColor(Pixel, 0, 0, 0);
}

// Custom Functions below

//Color in the pixels starting from pixle 0

void pixelFill(int pixelLength)
{
  for (int i = 0; i < pixelLength; i++)
  {
    strip.setPixelColor(i, pattern[0][0], pattern[0][1], pattern[0][2]);
    delay(hyper);
    strip.show();
  }
  for (int x = pixelLength; x <= strip.numPixels(); x++)
  {
    strip.setPixelColor(x, 0, 0 , 0);
  }
  strip.show();
}

void turnBlack()
{
  for (int x = 0; x <= strip.numPixels(); x++)
  {
    strip.setPixelColor(x, 0, 0 , 0);
    delay(sht);
    strip.show();
  }
}

//Cylon Function

void CylonEyeUp(uint32_t Co, uint32_t Ct, uint32_t Ctt, uint8_t Delay, int TotalPixels, int pStart) {
  for (int i = pStart; i < TotalPixels; i++) {
    if (!UsingBar) {
      strip.setPixelColor(i + 2, Ctt);  //Third Dot Color
    }
    strip.setPixelColor(i + 1, Ct); //Second Dot Color
    strip.setPixelColor(i, Co);     //Center Dot Color
    strip.setPixelColor(i - 1, Ct); //Second Dot Color
    if (!UsingBar) {
      strip.setPixelColor(i - 2, Ctt);  //Third Dot Color
    }

    if (!UsingBar) {
      strip.setPixelColor(i - 3, strip.Color(0, 0, 0)); //Clears the dots after the 3rd color
    } else {
      strip.setPixelColor(i - 2, strip.Color(0, 0, 0)); //Clears the dots after the 2rd color
    }
    strip.show();
    //Serial.println(i); //Used For pixel Count Debugging
    delay(Delay);
  }
}
void CylonEyeDown(uint32_t Co, uint32_t Ct, uint32_t Ctt, uint8_t Delay, int TotalPixels, int pEnd) {
  for (int i = TotalPixels - 1; i > pEnd; i--) {
    if (!UsingBar) {
      strip.setPixelColor(i - 2, Ctt);  //Third Dot Color
    }
    strip.setPixelColor(i - 1, Ct); //Second Dot Color
    strip.setPixelColor(i, Co);    //Center Dot Color
    strip.setPixelColor(i + 1, Ct); //Second Dot Color
    if (!UsingBar) {
      strip.setPixelColor(i + 2, Ctt);  //Third Dot Color
    }

    if (!UsingBar) {
      strip.setPixelColor(i + 3, strip.Color(0, 0, 0)); //Clears the dots after the 3rd color
    } else {
      strip.setPixelColor(i + 2, strip.Color(0, 0, 0)); //Clears the dots after the 2rd color
    }
    strip.show();
    //Serial.println(i); //Used For pixel Count Debugging
    delay(Delay);
  }
}

//Seizure Function

void Seizure(unsigned int flashRate)
{  unsigned int sectorSize = strip.numPixels()/3;
    for (int i = 0; i <= 6; i++)
  {
    for (int j=0; j<=sectorSize; j++)
    {
    strip.setPixelColor(j, 255,255,255);
    strip.setPixelColor(j+sectorSize, 0, 0 ,0);
    strip.setPixelColor(j+2*sectorSize, 255,255,255);
    strip.show();
    delay(flashRate);
    }
    for (int k=0; k<=sectorSize; k++)
    {
    strip.setPixelColor(k, 0,0,0);
    strip.setPixelColor(k+sectorSize, 255,255,255);
    strip.setPixelColor(k+2*sectorSize, 0,0,0);
    strip.show();
    delay(flashRate);
  }
}
}
    ////Full color spectrum transition code !!!WARNING Pixel Killer Code if misused!!!
    //  This code will run all pixels through the entire spectrum simultaneously but at delays less than 5ms proved pixel lethal in time!!!

    void setColor(){
      // Start off with red.
        spectrumColor[0] = 255;
        spectrumColor[1] = 0;
        spectrumColor[2] = 0;
    
        // Choose the Colors to increment and decrement.
        for (int decColor = 0; decColor < 3; decColor += 1) {
          int incColor = decColor == 2 ? 0 : decColor + 1;
    
          // cross-fade the two Colors.
          for(int i = 0; i < 255; i += 1) {
            spectrumColor[decColor] -= 1;
            spectrumColor[incColor] += 1;
            for (int x = 0; x < strip.numPixels(); x++){
            strip.setPixelColor(x, spectrumColor[0], spectrumColor[1],spectrumColor[2]);
            }
            strip.show();
            buttonState = digitalRead(pushButton);
                if (buttonState==1) {
                break;
                }
            delay(100);
          }
        }
      }
    ////end of color transition code !!!End of Pixel Killer Code!!!

int Sample(){
  // collect data for 50 mS
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int minSampleSum = 0;
  unsigned int minSampleAverage = 1;
  unsigned int sampleNoise = 0;
  while (millis() - startMillis < sampleWindow)
  {
    //open while loop
    sample = analogRead(0);
    if (sample < 1024)  // toss out spurious readings
    {
      //open 1st if loop in while
      if (sample > signalMax)
      {
        //open 2nd if
        signalMax = sample;  // save just the max levels
      }//close 2nd if
      else if (sample < signalMin)
      {
        signalMin = (minSampleSum + sample) / minSampleAverage;
        minSampleSum = minSampleSum + sample;
        minSampleAverage++;
        // average the min levels to establish baseline noise.
      }//close 3rd if
    }//close 1st if
  }//close while loop
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
  
  //section below maps the signal from the microphone on to 12 options for LED effects

  int sound = constrain((volts * 100), 30, 155);//generate voltage value and constrain results.
  int soundValue = map(sound, 30, 155, 0, 11);// map constrained values range 0 to 11.
  
  
  Serial.print("The level is: ");
  Serial.print("volts= ");
  Serial.print (sound);
  Serial.print ("  level= ");
  Serial.println(soundValue);
  //for debugging

  return soundValue;
  }

//end
