#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Math.h>
#include <Wtv020sd16p.h>
//#include <time.h>

//audio stuff
int resetPin = 2;  // The pin number of the reset pin.
int clockPin = 3;  // The pin number of the clock pin.
int dataPin = 4;  // The pin number of the data pin.
int busyPin = 5;  // The pin number of the busy pin.

/*
Create an instance of the Wtv020sd16p class.
 1st parameter: Reset pin number.
 2nd parameter: Clock pin number.
 3rd parameter: Data pin number.
 4th parameter: Busy pin number.
 */
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

//color enums
enum Debug {NONE,BASIC,DISTANCE,DEEP_DIST,CALIB};
enum Color {BLACK,RED,BLUE,GREEN,WHITE};
enum Country {ASIA,AMERICA,AFRICA,EUROPE,AUSTRALIA};

int debug = 0;
int colorR[5]={100,182,66,63,80};
int colorG[5]={93,50,82,123,99};
int colorB[5]={64,39,109,65,70};
int prevCountry = 4;

char *colorOf[5] ={"BLACK","RED","BLUE","GREEN","WHITE"};

char serialinput;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

float distance(int color1R, int color1G, int color1B, int color2){
  if (debug == DEEP_DIST) {
    Serial.print("(");
    Serial.print(color1R);
    Serial.print(",");
    Serial.print(color1G);
    Serial.print(",");
    Serial.print(color1B);
    Serial.print(") vs ");
    Serial.print(color2);
    Serial.print(": ");
  }
  
  float step1 = powf((color1R-colorR[color2]),2);

  float step2 = powf((color1G-colorG[color2]),2);

  float step3 = powf((color1B-colorB[color2]),2);

  float dist = sqrt(step1+step2+step3);

  if (debug == DEEP_DIST) {
    Serial.print("Step 1: ");
    Serial.println(step1);
    Serial.print("Step 2: ");
    Serial.println(step2);
    Serial.print("Step 3: ");
    Serial.println(step3);
    Serial.print("dist: ");
    Serial.println(dist);
  }
  
  return dist;
}

int minDistance(int color1R,int color1G, int color1B){
  int colorClosest;
  float dist=INFINITY;
  if (debug >= BASIC) {
    Serial.println("**START**");
  }
  for (int color=BLACK;color<=WHITE;color++){
    float d = distance(color1R,color1G,color1B,color);
    if (debug == DISTANCE) {
      Serial.println(color);
      Serial.println(d);
      Serial.println("====");
    }
    if (d<dist){
      dist = d;
      if (debug >= BASIC) {
        Serial.println(color);
      }
      colorClosest=color;
      if (debug >= BASIC) {
        Serial.println(color);
      }
    }
  }
  if (debug > BASIC) {
    Serial.println(colorClosest);
  }
  return colorClosest;
}

int countryTrack(int country) {
  //srand(time(NULL));
  switch (country){
    case ASIA:
      Serial.println("Asia");
      return (rand() % 3);
      break;
    case EUROPE:
      Serial.println("Europe");
      return (rand()% 2)+3;
      break;
    case AMERICA:
      Serial.println("America");
      return (rand()%3)+5;
      break;
    case AFRICA:
      Serial.println("Africa");
      //return (rand()%2)+8;
      return (rand()%3);
      break;
    default:
      Serial.println("default");
      return 0;
      break;
  }
}

void playAudio(int country){
  //Serial.println(country);
  if (prevCountry == country) {
    return;
  }
  else if (country != WHITE) {
    prevCountry = country;
    wtv020sd16p.asyncPlayVoice(countryTrack(country));
  }
  else {
    prevCountry = country;
    wtv020sd16p.stopVoice();
  }
}

void setup() {
  Serial.begin(9600);

  wtv020sd16p.reset();
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
}


void loop() {
  delay(1000);
  Serial.println("Detecting.");

    
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED

  // Figure out some basic hex code for visualization
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  
  int min[5];
  int res[5]={0,0,0,0,0};
  
  for (int i=0; i<5; i++){
    min[i] = minDistance((int)r,(int)g,(int)b);
    ++res[min[i]];
    if (debug == DEEP_DIST) {
      Serial.println(min[i]);
    }
  }
  
  int m = 0;
  int maxVal=-1;
  for (int i=0; i<5;i++) {
    if (res[i] > m){
      m=res[i];
      maxVal=i;
    }
  }
  Serial.println("*~*~*COLOR DETECTED*~*~*");
  Serial.println(colorOf[maxVal]);
  playAudio(maxVal);
}

