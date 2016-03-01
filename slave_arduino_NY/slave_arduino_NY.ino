
// LEDs
//Neopixels
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//WS2801 pixels
#include "SPI.h"
#include "Adafruit_WS2801.h"

uint8_t dataPin  = 4;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

#define NEOPIN 9
//Initialise strip objects
Adafruit_NeoPixel neoStrip = Adafruit_NeoPixel(5, NEOPIN, NEO_GRB + NEO_KHZ800);
Adafruit_WS2801 longStrip = Adafruit_WS2801(25, dataPin, clockPin);



//Perlin noise
#include <math.h>

//persistence affects the degree to which the "finer" noise is seen
float persistence = 0.25;
//octaves are the number of "layers" of noise that get computed
int octaves = 1;


// PIR
int inputPin = 6; // choose the input pin (for PIR sensor)
int pirState = LOW; // we start, assuming no motion detected
int val = 0; // variable for reading the pin status

boolean partyMode = false;
String partyColor = "blue";
long savedTime;
long randomTime;

// Communication with master
#include <Wire.h>

int cutoff;
int activityDelay = 1; 

String color = "";
uint32_t c; 

int redOffset, greenOffset, blueOffset, redOffsetNeo, greenOffsetNeo, blueOffsetNeo = 0;


void setup() {

  // Init communication with master
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  // Something something LEDs ?
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif

  // Init LEDs
  longStrip.begin();
  neoStrip.begin();
  longStrip.show();
  neoStrip.show();

  randomTime = random(3000, 5000);
}

void loop() {

  // If the partymode tag has been activated
  if(partyMode == true && millis() - savedTime >= randomTime) {
    savedTime = millis();
    randomTime = random(3000, 5000);

    if(partyColor == "blue") {
      partyColor = "orange";
    } else {
      partyColor = "blue";
    }

    changeColor(partyColor);
  }
  
  float rnd = float(millis())/100.0f;

  val = digitalRead(inputPin); // read input value
  
  cutoff += (val == HIGH) ? 1 : -1;
  cutoff = max(min(cutoff, 100 * activityDelay), 0);
  int activity = cutoff / activityDelay;
   
  Serial.println(activity); 
  for (int i=0; i < longStrip.numPixels(); i++) {
    //y2 = map(PerlinNoise2(i,y1,persistence,octaves), -1.0, 1.0, 50.0, 255.0);
    float contrast = PerlinNoise2(i,rnd,persistence,octaves)*128+128;
    
    // aendre contrast så den passer til aktivitets-niveauet
    contrast = max(contrast-activity, 0);
    contrast = min(contrast*((255+activity)/(255-activity)), 255);
    contrast = map(contrast, 0, 255, 1, 55+activity*2);
    
    byte r = max(0,min(255,contrast * redOffset));
    byte g = max(0,min(255,contrast * greenOffset));
    byte b = max(0,min(255,contrast * blueOffset));
    
    longStrip.setPixelColor(i,Color(r,g,b));
  }
  
  for (int i=0; i < neoStrip.numPixels(); i++) {
    //if (val == HIGH) {
      //y2 = map(PerlinNoise2(i,y1,persistence,octaves), -1.0, 1.0, 50.0, 255.0);
      float contrast = PerlinNoise2(i,rnd,persistence,octaves)*128+128;
      
      // aendre contrast så den passer til aktivitets-niveauet
      contrast = max(contrast-activity, 0);
      contrast = min(contrast*((255+activity)/(255-activity)), 255);
      contrast = map(contrast, 0, 255, 1, 55+activity*2);
      
      byte r = max(0,min(255,contrast * redOffsetNeo ));
      byte g = max(0,min(255,contrast * greenOffsetNeo ));
      byte b = max(0,min(255,contrast * blueOffsetNeo ));
    
      neoStrip.setPixelColor(i,Color(r,g,b));
   //}
    
   //else {
   // neoStrip.setPixelColor(i,Color(0,0,0));
   //}
  }    

  longStrip.show();
  neoStrip.show();
  //analogWrite(6,n);
  delay(1);
}

// Function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  color = "";
  while (Wire.available()) {
    // receive byte as a character
    char c = Wire.read();
    // Add char to String, so we'll have the full string in the end
    color += c; 
  }

  Serial.println(color);

  if(color == "partymode") {
    partyMode = true;
  } else {
    // Turn on LEDs in the recieved color
    partyMode = false;
    changeColor(color);
  }
  
}


// Function to light the LEDs in the recieved color
void changeColor(String colorName) {

  if(colorName == "red") {
    redOffset = 1.5;
    greenOffset = 0.75;
    blueOffset = 0.75;

    redOffsetNeo = 1.15;
    greenOffsetNeo = 0.0;
    blueOffsetNeo = 1.05;
  
  } else if(colorName == "green") {
    redOffset = 0.75;
    greenOffset = 1.5;
    blueOffset = 0.75;

    redOffsetNeo = 1;
    greenOffsetNeo = 1.4;
    blueOffsetNeo = 0.6;
  
  } else if(colorName == "blue") {
    redOffset = 0.75;
    greenOffset = 0.75;
    blueOffset = 1.5;

    redOffsetNeo = 0.75;
    greenOffsetNeo = 1.5;
    blueOffsetNeo = 0.75;

  } else if(colorName == "orange") {
    redOffset = 1.7;
    greenOffset = 1.3;
    blueOffset = 0;

    redOffsetNeo = 1.5;
    greenOffsetNeo = 0.75;
    blueOffsetNeo = 0.75;

  } else if(colorName == "purple") {
    redOffset = 1.15;
    greenOffset = 0.8;
    blueOffset = 1.05;

    redOffsetNeo = 0.75;
    greenOffsetNeo = 0.75;
    blueOffsetNeo = 1.5;
  
  } else if(colorName == "white") {
    redOffset = 1;
    greenOffset = 1;
    blueOffset = 1;
  }
}


float PerlinNoise2(float x, float y, float persistance, int octaves) {
  float frequency, amplitude;
  float total = 0.0;

  for (int i = 0; i <= octaves - 1; i++)
  {
    frequency = pow(2,i);
    amplitude = pow(persistence,i);

    total = total + InterpolatedNoise2(x * frequency, y * frequency) * amplitude;
  }

  return(total);
}

float InterpolatedNoise2(float x, float y) {
  float v1,v2,v3,v4,i1,i2,fractionX,fractionY;
  long longX,longY;

  longX = long(x);
  fractionX = x - longX;

  longY = long(y);
  fractionY = y - longY;

  v1 = SmoothNoise2(longX, longY);
  v2 = SmoothNoise2(longX + 1, longY);
  v3 = SmoothNoise2(longX, longY + 1);
  v4 = SmoothNoise2(longX + 1, longY + 1);

  i1 = Interpolate(v1 , v2 , fractionX);
  i2 = Interpolate(v3 , v4 , fractionX);

  return(Interpolate(i1 , i2 , fractionY));
}

float Interpolate(float a, float b, float x) {
  //cosine interpolations
  return(CosineInterpolate(a, b, x));
}

float CosineInterpolate(float a, float b, float x) {
  float ft = x * 3.1415927;
  float f = (1 - cos(ft)) * .5;

  return(a*(1-f) + b*f);
}

float SmoothNoise2(float x, float y) {
  float corners, sides, center;
  corners = ( Noise2(x-1, y-1)+Noise2(x+1, y-1)+Noise2(x-1, y+1)+Noise2(x+1, y+1) ) / 16;
  sides   = ( Noise2(x-1, y)  +Noise2(x+1, y)  +Noise2(x, y-1)  +Noise2(x, y+1) ) /  8;
  center  =  Noise2(x, y) / 4;
  return (corners + sides + center);
}

float Noise2(float x, float y) {
  long noise;
  noise = x + y * 57;
  noise = (noise << 13) ^ noise;
  return ( 1.0 - ( long(noise * (noise * noise * 15731L + 789221L) + 1376312589L) & 0x7fffffff) / 1073741824.0);
}


// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b) {
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

