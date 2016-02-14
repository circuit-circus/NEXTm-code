
// LEDs
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIN 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, NEOPIN, NEO_GRB + NEO_KHZ800);

// Communication with master
#include <Wire.h>


String color = "";
uint32_t c; 


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
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  delay(100);
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

  // Turn on LEDs in the recieved color
  lightLED(color);
}

// Function to light the LEDs in the recieved color
void lightLED(String colorName) {

  if(colorName == "red") {
    c = strip.Color(255, 0, 0);
  
  } else if(colorName == "green") {
    c = strip.Color(0, 255, 0);
  
  } else if(colorName == "blue") {
    c = strip.Color(0, 0, 255);
  
  } else if(colorName == "white") {
    c = strip.Color(255, 255, 255);
  }

  // Loop though LEDs and light them
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(50);
  }

}

