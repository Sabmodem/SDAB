#include "Arduino.h"
#include <SPI.h>
#include <SD.h>
// #include "reader.cc"
#include "browser.cpp"

// #include "browser.h"
// #include "browser.cc"

#define CARD_CS      10

browser* brw = nullptr;

void buttonsHandler(char cmd, browser* brw) {
  switch(cmd) {
  case 'n':
    brw->moveCurfileUp();
    brw->print();
    break;
  case 'p':
    brw->moveCurfileDown();
    brw->print();
    break;
  };
};

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing SD card...");

  // if( !SD.begin( CARD_CS )){
  //   Serial.println("initialization failed!");
  //   return;
  // }
  // browserInit();
  brw = new browser();
  // browser brw();
  brw->print();
}

void loop() {
  if(Serial.available()) {
    buttonsHandler(Serial.read(), brw);
  };
}
