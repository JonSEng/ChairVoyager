#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Math.h>
#include <Wtv020sd16p.h>
//#include <time.h>

//FOR WIFI CONNECTION
#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>

////////////////////////////////////
// CC3000 Shield Pins & Variables //
////////////////////////////////////
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       6   // Can be any digital pin
#define CC3000_CS       10  // Preferred is pin 10 on Uno
#define IP_ADDR_LEN     4   // Length of IP address in bytes

////////////////////
// WiFi Constants //
////////////////////
char ap_ssid[] = "secret";                // SSID of network
char ap_password[] = "invention";        // Password of network
unsigned int ap_security = WLAN_SEC_WPA2; // Security of network
// ap_security can be any of: WLAN_SEC_UNSEC, WLAN_SEC_WEP, 
//  WLAN_SEC_WPA, or WLAN_SEC_WPA2
unsigned int timeout = 30000;             // Milliseconds
char server[] = "data.sparkfun.com";      // Remote host site

// Initialize the CC3000 objects (shield and client):
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client client = SFE_CC3000_Client(wifi);

/////////////////
// Phant Stuff //
/////////////////
const String publicKey = "Jx3n8mWoYEsNwYYDwWM0";
const String privateKey = "gzyxbaPDopiWVxxgVdBZ";
const byte NUM_FIELDS = 1;
const String fieldNames[NUM_FIELDS] = {"url"};
String fieldData[NUM_FIELDS];

//////////////////////
// Input Pins, Misc //
//////////////////////
const int triggerPin = 3;
const int lightPin = A0;
const int switchPin = 5;
String name = "Anonymouse";
boolean newName = true;

//article array

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
enum Country {EUROPE,ASIA,AMERICA,AFRICA,AUSTRALIA};

//int debug = DEEP_DIST;
int colorR[5]={100,182,66,63,80};
int colorG[5]={93,50,82,123,99};
int colorB[5]={64,39,109,65,70};
int prevCountry = 4;

char *colorOf[5] ={"BLACK","RED","BLUE","GREEN","WHITE"};

char serialinput;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

float distance(int color1R, int color1G, int color1B, int color2){
  float step1 = powf((color1R-colorR[color2]),2);

  float step2 = powf((color1G-colorG[color2]),2);

  float step3 = powf((color1B-colorB[color2]),2);

  float dist = sqrt(step1+step2+step3);
  return dist;
}

int minDistance(int color1R,int color1G, int color1B){
  int colorClosest;
  float dist=INFINITY;

  for (int color=BLACK;color<=WHITE;color++){
    float d = distance(color1R,color1G,color1B,color);
    if (d<dist){
      dist = d;
      colorClosest=color;
    }
  }

  return colorClosest;
}

int countryTrack(int country) {
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
    int trackNum = countryTrack(country);
    Serial.println("tracknum: ");
    Serial.println(trackNum);
//    fieldData[0] = articleURLs[trackNum];
    fieldData[0] = String(trackNum);
    postData();
    wtv020sd16p.asyncPlayVoice(trackNum);
  }
  else {
    prevCountry = country;
    wtv020sd16p.stopVoice();
  }
}

//for wifi posting
void postData()
{
  Serial.println("In postData()");
  // Make a TCP connection to remote host
  if ( !client.connect(server, 80) )
  {
    // Error: 4 - Could not make a TCP connection
    Serial.println(F("Error: 4"));
  }

  Serial.println("postData(): client.printing");
  // Post the data! Request should look a little something like:
  // GET /input/publicKey?private_key=privateKey&light=1024&switch=0&time=5201 HTTP/1.1\n
  // Host: data.sparkfun.com\n
  // Connection: close\n
  // \n
  client.print("GET /input/");
  client.print(publicKey);
  client.print("?private_key=");
  client.print(privateKey);
  Serial.println("Printing fieldNames,fieldData="+fieldNames[0]+fieldData[0]);
  for (int i=0; i<NUM_FIELDS; i++)
  {
    client.print("&");
    client.print(fieldNames[i]);
    client.print("=");
    client.print(fieldData[i]);
  }
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(server);
  client.println("Connection: close");
  client.println();
  Serial.println("Connection closed");

  while (client.connected())
  {
    if ( client.available() )
    {
      char c = client.read();
      Serial.print(c);
    }      
  }
  Serial.println();
}

void setupWiFi()
{
  ConnectionInfo connection_info;
  int i;

  // Initialize CC3000 (configure SPI communications)
  if ( wifi.init() )
  {
    Serial.println(F("CC3000 Ready!"));
  }
  else
  {
    // Error: 0 - Something went wrong during CC3000 init!
    Serial.println(F("Error: 0"));
  }

  // Connect using DHCP
  Serial.print(F("Connecting to: "));
  Serial.println(ap_ssid);
  if(!wifi.connect(ap_ssid, ap_security, ap_password, timeout))
  {
    // Error: 1 - Could not connect to AP
    Serial.println(F("Error: 1"));
  }

  // Gather connection details and print IP address
  if ( !wifi.getConnectionInfo(connection_info) ) 
  {
    // Error: 2 - Could not obtain connection details
    Serial.println(F("Error: 2"));
  }
  else
  {
    Serial.print(F("My IP: "));
    for (i = 0; i < IP_ADDR_LEN; i++)
    {
      Serial.print(connection_info.ip_address[i]);
      if ( i < IP_ADDR_LEN - 1 )
      {
        Serial.print(".");
      }
    }
    Serial.println();
  }
}




void setup() {
  Serial.begin(115200);

  wtv020sd16p.reset();
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  
  setupWiFi();
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
