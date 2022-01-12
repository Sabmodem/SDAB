#include <Arduino.h>
#include <SD.h>
#include "classes.cc"

display* ds = nullptr;
browser* br = nullptr;
reader* rd = nullptr;
bool state = true; // Режим работы устройства. 1 - браузер, 0 - читалка

void buttonsHandler(char cmd, browser* br) {
  switch (cmd) {
  case 'n': // Если нажата n
    if(state) {
      br->moveCurfileUp(); // читаем вперед
      // br->print();
    } else {
      rd->readPageForward();
      // rd->print();
    };
    break;
  case 'p': // если нажата p
    if(state) {
      br->moveCurfileDown(); // читаем вперед
      // br->print();
    } else {
      rd->readPageBackward();
      // rd->print();
    };
    break;
  case 's': // если нажата p
    if(state) {
      String* file = new String(*br->getCurfile()); // Этот указатель используется в browser. Не удалять!
      delete br;
      br = nullptr;
      rd = new reader(*file);
      ds->setBufPtr(rd->buf);
      ds->setBufSize(6);
      state = !state;
    } else {
      delete rd;
      rd = nullptr;
      br = new browser();
      ds->setBufPtr(br->buf);
      ds->setBufSize(6);
    };
    break;
  };
};

void setup() {
  Serial.begin(9600);
  if( !SD.begin( 10 )){
  //   Serial.println("initialization failed!");
    return;
  }

  br = new browser();
  ds = new display(6, br->buf);
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
