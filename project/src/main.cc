#include <Arduino.h>
#include <SD.h>
#include "classes.cc"

#define card_cs 10
#define btn_a 3
#define btn_b 4
#define btn_c 5

display* ds = nullptr;
browser* br = nullptr;
reader* rd = nullptr;
bool state = true; // Режим работы устройства. 1 - браузер, 0 - читалка

unsigned long time = 0;

char btn() {
  if(millis() - time < 1000) {
    return -1;
  };
  char btn_state = 0;
  if(digitalRead(btn_a) == LOW) {
    btn_state = 1;
    // Serial.println("Вниз");
  } else if(digitalRead(btn_b) == LOW) {
    btn_state = 2;
    // Serial.println("ВВерх");
  } else if(digitalRead(btn_c) == LOW) {
    btn_state = 3;
    // Serial.println("Выбор");
  }
  time = millis();
  return btn_state;
}


void buttonsHandler(char cmd) {
  switch (cmd) {
  case 1:
    if(state) {
      br->moveCurfileUp();
    } else {
      rd->readPageForward();
    };
    break;
  case 2:
    if(state) {
      br->moveCurfileDown();
    } else {
      rd->readPageBackward();
    };
    break;
  case 3:
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
  pinMode(btn_a, INPUT_PULLUP);
  pinMode(btn_b, INPUT_PULLUP);
  pinMode(btn_c, INPUT_PULLUP);

  if( !SD.begin( card_cs )){
    return;
  }

  br = new browser();
  ds = new display(6, br->buf);
}

void loop() {
  buttonsHandler(btn());

  if(state) {
    ds->printDir(br->curfilePosInBuf());
  } else {
    ds->printPage();
  };
};
