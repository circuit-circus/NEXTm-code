
// RFID READER STUFF
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


// MASTER-SLAVE CONNECTION
#include <Wire.h>

String readId =  "";    // Variable integer to keep if we have Successful Read from Reader
String colorToSend = "";

byte readCard[4];    // Stores scanned ID read from RFID Module
boolean programMode = false;  // initialize programming mode to false


void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522

  Wire.begin(); // begin master-slave connection

}

void loop() {
  do {
    readId = getID();   // sets successRead to 1 when we get read from reader otherwise 0
    if(readId.length() > 0) {

      colorToSend = idToColor(readId);
     
      Wire.beginTransmission(8); // transmit to device #8
      //Wire.write(colorToSend.c_str());        // sends one byte
      Wire.write(colorToSend.c_str());        // sends one byte
      Wire.endTransmission();    // stop transmitting
      delay(500);
    }
  }
  while (readId.length() > 0);   // the program will not go further while you not get a successful read
}

String idToColor(String id) {


  if (id == "36962234") {
      return "green";
  } else if (id == "18020171235") {
      return "red";
  } else if (id == "31588246") {
      return "blue";
  } else if (id == "311688246") {
      return "white";
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
 
  mfrc522.PICC_HaltA(); // Stop reading
  return id;
}

