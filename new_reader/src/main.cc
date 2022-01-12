#include "Arduino.h"
#include <SD.h>
#include "display.cc"

reader* rd = nullptr;
display* ds = nullptr;

void buttonsHandler(char cmd, reader* rd) {
  switch(cmd) {
  case 'n':
    // str = rd->readStringForward();
    // Serial.println(*str);
    // rd->print();
    // delete str;

    rd->readPageForward();
    rd->print();
    break;
  case 'p':
    // str = rd->readStringBackward();
    // Serial.println(*str);
    // rd->print();
    // delete str;

    rd->readPageBackward();
    rd->print();
    break;
  };
};

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing SD card...");

  if( !SD.begin( 10 )){
    Serial.println("initialization failed!");
    return;
  }

  rd = new reader("1.txt");
  ds = new display(3, rd->buf);
  // for(char i = 0; i < 0; i++) {
  //   Serial.print("I: ");
  //   Serial.println(i, DEC);
  //   // rd->readPageForward();
  //   // rd->print();

  //   Serial.print("I: ");
  //   Serial.println(i, DEC);
  //   String* str = nullptr;
  //   str = rd->readStringForward();
  //   Serial.println(*str);
  //   rd->print();
  //   delete str;
  // };

  // for(char i = 0; i < 0; i++) {
  //   // Serial.print("I: ");
  //   // Serial.println(i, DEC);
  //   // rd->readPageBackward();
  //   // rd->print();

  //   Serial.print("I: ");
  //   Serial.println(i, DEC);
  //   String* str = nullptr;
  //   str = rd->readStringBackward();
  //   Serial.println(*str);
  //   rd->print();
  //   delete str;
  // };
}

void loop() {
  if(Serial.available()) {
    buttonsHandler(Serial.read(), rd);
  };
  ds->printPage();
}
