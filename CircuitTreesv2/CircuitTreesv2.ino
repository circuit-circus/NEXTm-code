/*
copyright 2007 Mike Edwards
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 */
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIN 9

#include <math.h>

#include "SPI.h"
#include "Adafruit_WS2801.h"

uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_NeoPixel neoStrip = Adafruit_NeoPixel(5, NEOPIN, NEO_GRB + NEO_KHZ800);
Adafruit_WS2801 longStrip = Adafruit_WS2801(25, dataPin, clockPin);

int inputPin = 6; // choose the input pin (for PIR sensor)
int pirState = LOW; // we start, assuming no motion detected
int val = 0; // variable for reading the pin status

byte leds[25];

//persistence affects the degree to which the "finer" noise is seen
float persistence = 0.25;
//octaves are the number of "layers" of noise that get computed
int octaves = 1;

int cutoff;
int activityDelay = 5; 


uint32_t c;

void setup()
{

  Serial.begin(9600);


  longStrip.begin();
  neoStrip.begin();
  longStrip.show();
  neoStrip.show();
}

void loop()
{
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
    
    byte r = contrast * (activity / 100.0);
    byte g = contrast * ((100 - activity) / 100.0);
    byte b = 0;
    
    longStrip.setPixelColor(i,Color(r,g,b));
  }
  
  for (int i=0; i < neoStrip.numPixels(); i++) {
    if (val == HIGH) {
      //y2 = map(PerlinNoise2(i,y1,persistence,octaves), -1.0, 1.0, 50.0, 255.0);
      float contrast = PerlinNoise2(i,rnd,persistence,octaves)*128+128;
      
      // aendre contrast så den passer til aktivitets-niveauet
      contrast = max(contrast-activity, 0);
      contrast = min(contrast*((255+activity)/(255-activity)), 255);
      contrast = map(contrast, 0, 255, 1, 55+activity*2);
      
      byte r = contrast * (activity / 100.0);
      byte g = contrast * ((100 - activity) / 100.0); 
      byte b = constrain((contrast * (activity / 100.0)),0,50);
    
      neoStrip.setPixelColor(i,Color(r,g,b));
   }
    
   else {
    neoStrip.setPixelColor(i,Color(0,0,0));
   }
  }    

  longStrip.show();
  neoStrip.show();
  //analogWrite(6,n);
  delay(1);
}


//using the algorithm from http://freespace.virgin.net/hugo.elias/models/m_perlin.html
// thanks to hugo elias
float Noise2(float x, float y)
{
  long noise;
  noise = x + y * 57;
  noise = (noise << 13) ^ noise;
  return ( 1.0 - ( long(noise * (noise * noise * 15731L + 789221L) + 1376312589L) & 0x7fffffff) / 1073741824.0);
}

float SmoothNoise2(float x, float y)
{
  float corners, sides, center;
  corners = ( Noise2(x-1, y-1)+Noise2(x+1, y-1)+Noise2(x-1, y+1)+Noise2(x+1, y+1) ) / 16;
  sides   = ( Noise2(x-1, y)  +Noise2(x+1, y)  +Noise2(x, y-1)  +Noise2(x, y+1) ) /  8;
  center  =  Noise2(x, y) / 4;
  return (corners + sides + center);
}

float InterpolatedNoise2(float x, float y)
{
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

float Interpolate(float a, float b, float x)
{
  //cosine interpolations
  return(CosineInterpolate(a, b, x));
}

float LinearInterpolate(float a, float b, float x)
{
  return(a*(1-x) + b*x);
}

float CosineInterpolate(float a, float b, float x)
{
  float ft = x * 3.1415927;
  float f = (1 - cos(ft)) * .5;

  return(a*(1-f) + b*f);
}

float PerlinNoise2(float x, float y, float persistance, int octaves)
{
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

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}



