#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Math.h>

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];

//enum Color1 {DBLUE,WHITE,BLACK,RED};
int colorR1[4]={247,197,236,158};
int colorG1[4]={238,240,213,248};
int colorB1[4]={221,252,242,250};

enum Color {BLACK,RED,BLUE,GREEN,WHITE};
int colorR[5]={100,182,66,63,80};
int colorG[5]={93,50,82,123,99};
int colorB[5]={64,39,109,65,70};

char *colorOf[5] ={"BLACK","RED","BLUE","GREEN","WHITE"};

char serialinput;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

float distance(int color1R, int color1G, int color1B, int color2){
//  Serial.print("(");
//  Serial.print(color1R);
//  Serial.print(",");
//  Serial.print(color1G);
//  Serial.print(",");
//  Serial.print(color1B);
//  Serial.print(") vs ");
//  Serial.print(color2);
//  Serial.print(": ");
  
  //Serial.print("Step 1: ");
  float step1 = powf((color1R-colorR[color2]),2);
  //Serial.println(step1);
  //Serial.print("Step 2: ");
  float step2 = powf((color1G-colorG[color2]),2);
  //Serial.println(step2);
  //Serial.print("Step 3: ");
  float step3 = powf((color1B-colorB[color2]),2);
  //Serial.println(step3);
  //Serial.print("dist: ");
  float dist = sqrt(step1+step2+step3);
  //Serial.println(dist);
  
  return dist;
}

int minDistance(int color1R,int color1G, int color1B){
  int colorClosest;
  float dist=INFINITY;
//  Serial.println("**START**");
  for (int color=BLACK;color<=WHITE;color++){
    //Serial.println(color);
    float d = distance(color1R,color1G,color1B,color);
//    Serial.println(color);
//    Serial.println(d);
//    Serial.println("====");
    if (d<dist){
      dist = d;
      //Serial.println(color);
      //Serial.println("LOL");
      colorClosest=color;
      //Serial.println("LOL2");
      //Serial.println(color);
      //Serial.println("LOL3");
    }
  }
  //Serial.println(colorClosest);
  return colorClosest;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  
  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;      
    }
    //Serial.println(gammatable[i]);
  }
}


void loop() {
  delay(1000);
  Serial.println("Ready to check. Input \'y\'");
//  while(Serial.available()==0);
//  serialinput=Serial.read();
//  if (serialinput=='y'){
    
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
      //Serial.println(min[i]);
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
//  }
  


//  for (int color=BLACK; color<=WHITE; color++){
//    Serial.println(color);
//    while(Serial.available()==0);
//    serialinput = Serial.read();
//    if (serialinput == 'y'){
//      for (int i=0; i<100; i++){
//        uint16_t clear, red, green, blue;
//
//        tcs.setInterrupt(false);      // turn on LED
//      
//        delay(60);  // takes 50ms to read 
//        
//        tcs.getRawData(&red, &green, &blue, &clear);
//      
//        tcs.setInterrupt(true);  // turn off LED
//      
//        // Figure out some basic hex code for visualization
//        uint32_t sum = clear;
//        float r, g, b;
//        r = red; r /= sum;
//        g = green; g /= sum;
//        b = blue; b /= sum;
//        r *= 256; g *= 256; b *= 256;
//
////        colorR[color] += gammatable[(int)r];
////        colorG[color] += gammatable[(int)g];
////        colorB[color] += gammatable[(int)b];   
//
//        colorR[color] += (int)r;
//        colorG[color] += (int)g;
//        colorB[color] += (int)b;
//      }
//      colorR[color] = colorR[color]/100;
//      colorG[color] = colorG[color]/100;
//      colorB[color] = colorB[color]/100;
//      Serial.println(colorR[color]);
//      Serial.println(colorG[color]);
//      Serial.println(colorB[color]);
//    }
//  }
}


