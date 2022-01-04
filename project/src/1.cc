#include <Arduino.h>
#include <SD.h>
#include "classes.cc"

display* ds = nullptr;
browser* br = nullptr;
reader* rd = nullptr;
bool state = true; // Режим работы устройства. 1 - браузер, 0 - читалка

void buttonsHandler(char cmd, browser* br) {
  // char _curfilePosInBuf = br->curfilePosInBuf();
  switch (cmd) {
  case 'n': // Если нажата n
    if(state) {
      br->moveCurfileUp(); // читаем вперед
      br->print();
    } else {
      rd->readPageForward();
    };
    // buf = br->getBuf();
    break;
  case 'p': // если нажата p
    if(state) {
      br->moveCurfileDown(); // читаем вперед
      br->print();
    } else {
      rd->readPageBackward();
    };
    // buf = br->getBuf();
    break;
  case 's': // если нажата p
    String* file = new String(*br->getCurfile());
    delete br;
    br = nullptr;
    rd = new reader(*file);
    ds->setBufPtr(rd->buf);
    state = !state;
    ds->setState(state);
    // br->moveCurfileDown(); // читаем вперед
    // br->print();
    // buf = br->getBuf();
    break;
  };
};

void setup() {
  Serial.begin(9600);
  if( !SD.begin( 10 )){
    Serial.println("initialization failed!");
    return;
  }

  br = new browser();
  ds = new display(br->buf, state);
}

void loop() {
  if(Serial.available()) {
    buttonsHandler(Serial.read(), br);
  };

  if(state) {
    ds->printDir(br->curfilePosInBuf());
  } else {
    ds->printPage();
  };
};
