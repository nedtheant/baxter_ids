/*
  Firmware (firmware.ino) for a card reader used to keep track of attendance at
  events.

  To be used in conjunction with main.exe in software through serial 
  communication.

  Written by Ned Worboys (z5417857) on 26/1/2023
*/ 

#include <EEPROM.h>
#include <MFRC522.h>
#include <SPI.h>

// PIN DEFINITIONS (SPI pins in readme)
#define GREEN_LED     8
#define RED_LED       7
#define RF_RST        9
#define RF_SS         10

MFRC522 rf(RF_SS, RF_RST);
String input_string = "";   
bool command_input = false;


void setup(void) {
  Serial.begin(9600);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  SPI.begin();
  rf.PCD_Init();

  flash(GREEN_LED, 100, 2);
}


void loop(void) {
  if (rf.PICC_IsNewCardPresent()) {
    record_card();
  }

  if (command_input) {
    if (input_string == "unknown_uid") {
      flash(RED_LED, 500, 3);
    } else if (input_string == "dump") {
      dump_eeprom();
    } else if (input_string == "clear") {
      clear_eeprom();
    }

    command_input = false;
    input_string = "";
  }
}


void serialEvent() {  // Called at the end of every main loop if data is available on serial.
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      command_input = true;
      return;
    }
    input_string += inChar;
  }
}


void record_card(void) {
  unsigned long uid = get_uid();
  if (uid == 0) {
    flash(RED_LED, 100, 2);
    return;
  }

  // Check if uid already in eeprom
   unsigned long r_uid;
  for (int eeAdr = 0; eeAdr < EEPROM.length(); eeAdr += sizeof(unsigned long)) {
    EEPROM.get(eeAdr, r_uid);
    if (r_uid == uid) {
      // Duplicate scan (long red)
      Serial.print(uid);
      Serial.println(" has already scanned in.");
      flash(RED_LED, 300, 1);
      return;
    } else if (r_uid == 0) {
      // Good scan (long green), add to eeprom
      Serial.print(uid);
      Serial.println(" welcome to coffee night.");
      flash(GREEN_LED, 300, 1);
      EEPROM.put(eeAdr, uid); 
      return;
    } 
  }
  // Ran out of memory.
  flash(RED_LED, 300, 10);
}


void flash(int pin, int del, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(del);
    digitalWrite(pin, LOW);
    delay(del);
  }
}


void clear_eeprom(void) {
  unsigned long f = 0;
  for (int eeAdr = 0; eeAdr < EEPROM.length(); eeAdr += sizeof(unsigned long)) {
    EEPROM.put(eeAdr, f);
  }
  Serial.println("Memory Cleared.");
}

void dump_eeprom(void) {
  Serial.println("EEPROM contents:");
  unsigned long f;
  for (int eeAdr = 0; eeAdr < EEPROM.length(); eeAdr += sizeof(unsigned long)) {
    EEPROM.get(eeAdr, f);
    Serial.print(f);
    Serial.print(" ");
  }
  Serial.println("End EEPROM");
}

unsigned long get_uid(void) {
  if (!rf.PICC_ReadCardSerial()) {
    return 0;  // return 0 for empty or error.
  }
  unsigned long uid;
  uid =  rf.uid.uidByte[0] << 24;
  uid += rf.uid.uidByte[1] << 16;
  uid += rf.uid.uidByte[2] <<  8;
  uid += rf.uid.uidByte[3];
  rf.PICC_HaltA(); // Stop reading
  return uid;
}
