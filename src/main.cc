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

void(* resetFunc) (void) = 0;
/*
  Функция для перезагрузки контроллера. Сбрасывает счетчик команд на ноль,
  тем самым имитируя перезагрузку контроллера. Все настройки при этом
  остаются теми же самыми. Используется при переходе из режима браузера в
  режим читалки. На этом моменте, судя по всему, кончается память и
  начинается всякая жопа. Поэтому я предпочел просто использовать такой
  костыль. Может быть, исправлю в будущем
*/

char btn() {
  if(millis() - time < 1000) {
    return -1;
  };
  char btn_state = 0;
  if(digitalRead(btn_a) == LOW) {
    btn_state = 1;
  } else if(digitalRead(btn_b) == LOW) {
    btn_state = 2;
  } else if(digitalRead(btn_c) == LOW) {
    btn_state = 3;
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
      // String* file = new String(*br->getCurfile()); // Этот указатель используется в browser. Не удалять!
      rd = new reader(*br->getCurfile());
      delete br;
      br = nullptr;
      ds->setBufPtr(rd->buf);
      ds->setBufSize(6);
      state = !state;
    } else {
      // delete rd;
      // rd = nullptr;
      // br = new browser();
      // ds->setBufPtr(br->buf);
      // ds->setBufSize(6);
      resetFunc();
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
}
