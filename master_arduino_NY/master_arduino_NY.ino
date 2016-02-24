
// RFID READER STUFF
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


// LEDs
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NEOPIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, NEOPIN, NEO_GRB + NEO_KHZ800);


// MASTER-SLAVE CONNECTION
#include <Wire.h>

String readId =  "";    // Variable integer to keep if we have Successful Read from Reader
String colorToSend = "";


String color = "";
uint32_t c;

byte readCard[4];    // Stores scanned ID read from RFID Module
boolean programMode = false;  // initialize programming mode to false

long savedTime;
boolean counting;
boolean shouldTurnOn;

void setup() {
  Serial.begin(9600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();		// Init MFRC522

  Wire.begin(); // begin master-slave connection



  // Something something LEDs ?
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  // Init LEDs
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {
  do {
    readId = getID();   // sets successRead to 1 when we get read from reader otherwise 0
    if (readId.length() > 0) {

      colorToSend = idToColor(readId);
      Serial.println(colorToSend);

      Wire.beginTransmission(8); // transmit to device #8
      //Wire.write(colorToSend.c_str());        // sends one byte
      Wire.write(colorToSend.c_str());        // sends one byte
      Wire.endTransmission();    // stop transmitting
      delay(500);
    }
  }
  while (readId.length() > 0);   // the program will not go further while you not get a successful read

  if ( millis() - savedTime >= 5000 && counting == true && shouldTurnOn == false) {
    counting = false;
    shouldTurnOn = true;
    fadeLED(255, 255, 0, 255, 0, 255, 100);
    delay(5000);
  }
}

String idToColor(String id) {

  if (id == "4264518") {
    lightLED("green");
    return "green";

  } else if (id == "4234518") {
    lightLED("red");
    return "red";

  } else if (id == "4224518") {
    lightLED("blue");
    return "blue";

  } else if (id == "4194518") {
    lightLED("purple");
    return "purple";

  } else if (id == "4214518") {
    lightLED("orange");
    return "orange";

  } else {
    return "no color matched";
  }
}

String getID() {

  String id = "";

  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return id;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return id;
  }

  for (int i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    id += readCard[i];
  }

  Serial.println(id);

  mfrc522.PICC_HaltA(); // Stop reading
  return id;
}


// Function to light the LEDs in the sent color
void lightLED(String colorName) {
 
  shouldTurnOn = false;

  if (colorName == "red") {
    c = strip.Color(255, 0, 0);
    fadeLED(255, 255, 255, 0, 255, 0, 30);


  } else if (colorName == "green") {
    //c = strip.Color(0, 255, 0);

    fadeLED(255, 0, 255, 255, 255, 0, 30);

  } else if (colorName == "blue") {
    //c = strip.Color(0, 0, 255);

    fadeLED(255, 0, 255, 0, 255, 255, 30);

  } else if (colorName == "purple") {
    //c = strip.Color(180, 10, 230);

    fadeLED(255, 180, 255, 10, 255, 230, 30);

  } else if (colorName == "orange") {
    //c = strip.Color(245, 210, 10);

    fadeLED(255, 245, 255, 210, 255, 10, 30);

  } else {
    //c = strip.Color(255, 255, 255);
    fadeLED(255, 255, 255, 255, 255, 255, 30);
  }

  /*
    // Loop though LEDs and light them
    for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    }
    strip.show();
  */
}

void fadeLED(int Rstart, int Rend, int Gstart, int Gend, int Bstart, int Bend, int fadeTime) {

  savedTime = millis();
  counting = true;

  int n = fadeTime;
  for (int i = 0; i < n; i++) { // larger values of 'n' will give a smoother/slower transition.
    int Rnew = Rstart + (Rend - Rstart) * i / n;
    int Gnew = Gstart + (Gend - Gstart) * i / n;
    int Bnew = Bstart + (Bend - Bstart) * i / n;

    for (uint16_t j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, Rnew, Gnew, Bnew);
    }
    strip.show();
    delay(10);
  }
}

