/*
  Firmware (firmware.ino) for a card reader used to keep track of attendance at
  events.

  To be used in conjunction with main.exe in software through serial 
  communication.

  Written by Ned Worboys (z5417857) on 26/1/2023
*/ 

#include <EEPROM.h>
#include <SoftwareSerial.h>

// PIN DEFINITIONS
#define GREEN_LED     8
#define RED_LED       7
#define BLUE_LED      6
#define TX            4   // Connect to scanner RX
#define RX            3   // Connect to scanner TX

SoftwareSerial b_scan(RX, TX);
String input_string = "";   
bool command_input = false;
String barcode = "";


void setup(void) {
  Serial.begin(9600);
  b_scan.begin(9600);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(GREEN_LED, HIGH);  // Active low
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
  
  flash(GREEN_LED, 100, 2);   // Flash green twice signalling finish setup
}


void loop(void) {
  if (b_scan.available()) {
    softwareSerialEvent();
    record_card();
  }
    
  if (command_input) {
    if (input_string == "dump") {
      dump_eeprom();
    } else if (input_string == "clear") {
      clear_eeprom();
    }

    command_input = false;
    input_string = "";
  }
}


void softwareSerialEvent() {
  barcode = "";
  while (b_scan.available()) {
    char inChar = (char)b_scan.read();
    barcode += inChar;
    delay(5);
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
  
  unsigned long zid = get_zid();
  if (zid == 0) {
    Serial.println("Bad barcode.");
    flash(RED_LED, 100, 2);
    return;
  }

  // Check if zid already in eeprom
  unsigned long r_zid;
  for (int eeAdr = 0; eeAdr < EEPROM.length(); eeAdr += sizeof(unsigned long)) {
    EEPROM.get(eeAdr, r_zid);
    if (r_zid == zid) {
      // Duplicate scan (long red)
      Serial.print(zid);
      Serial.println(" has already scanned in.");
      flash(RED_LED, 300, 1);
      return;
    } else if (r_zid == 0) {
      // Good scan (long green), add to eeprom
      Serial.print(zid);
      Serial.println(" welcome to coffee night.");
      flash(GREEN_LED, 300, 1);
      EEPROM.put(eeAdr, zid); 
      return;
    } 
  }
  // Ran out of memory.
  flash(RED_LED, 300, 10);
}


unsigned long get_zid() {
  if (barcode.length() != 14) {
    return 0;
  }
  return barcode.substring(2, 9).toInt();
}


void flash(int pin, int del, int times) {
  digitalWrite(BLUE_LED, HIGH);
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, LOW);
    delay(del);
    digitalWrite(pin, HIGH);
    delay(del);
  }
  digitalWrite(BLUE_LED, LOW);
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
