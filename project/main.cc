// #include "reader.cc"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
// U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

// #include "reader.cc"
#include "browser.h"
#include "browser.cc"



// reader* rd = nullptr;
// browser* brw = nullptr;

// void buttonsHandlerR(char cmd, reader* rd) {
//   switch (cmd) {
//   case 'n': // Если нажата n
//     rd->readPageForward(); // читаем вперед
//     rd->print();
//     break;
//   case 'p': // если нажата p
//     rd->readPageBackward(); // читаем назад
//     rd->print();
//     break;
//   };
// };

browser brw;

void buttonsHandlerB(char cmd, browser brw) {
  switch(cmd) {
  case 'n':
    brw.moveCurfileUp();
    brw.printO();
    // brw->print();
    break;
  case 'p':
    brw.moveCurfileDown();
    brw.printO();
    // brw->print();
    break;
  };
};

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing SD card...");

  if( !SD.begin( CARD_CS )){
    Serial.println("initialization failed!");
    return;
  }
  // browserInit();
  // brw = new browser();
  // browser brw();
  // brw = browser();
  brw.print();
  // brw->printO();

  // rd = new reader("1.txt");
};

void loop() {
  if(Serial.available()) {
    buttonsHandlerB(Serial.read(), brw);
  };

  u8g.firstPage();
  do {
    brw.printO();
  } while( u8g.nextPage() );
};
