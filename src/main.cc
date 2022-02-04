#include <Arduino.h>
#include <SD.h>
#include "classes.cc"

#define bufsize_global 4

#define card_cs 9
// #define btn_a 3
// #define btn_b 4
// #define btn_c 5

#define PIN_BUTTON_1 5
#define PIN_BUTTON_2 3

bool     button_state_1      = false;
bool     button_state_2      = false;

bool     button_long_state_1 = false;
bool     button_long_state_2 = false;

uint32_t ms_button_1 = 0;
uint32_t ms_button_2 = 0;

char btn_1_handler() {
  uint32_t ms    = millis();
  bool pin_state = digitalRead(PIN_BUTTON_1);
  char cmd = -1;
  // Фиксируем нажатие кнопки
  if( pin_state  == LOW && !button_state_1 && ( ms - ms_button_1 ) > 50 ){
    button_state_1      = true;
    button_long_state_1 = false;
    ms_button_1         = ms;
  }
  // Фиксируем длинное нажатие кнопки
  if( pin_state  == LOW && !button_long_state_1 && ( ms - ms_button_1 ) > 1000 ){
    button_long_state_1 = true;
    // Serial.println("Выбор");
    cmd = 3;
  }
  // Фиксируем отпускание кнопки
  if( pin_state == HIGH && button_state_1 && ( ms - ms_button_1 ) > 50  ){
    button_state_1      = false;
    ms_button_1         = ms;
    if( !button_long_state_1 ) {
      // Serial.println("Вверх");
      cmd = 2;
    };
  }
  return cmd;
};

char btn_2_handler() {
  uint32_t ms    = millis();
  bool pin_state = digitalRead(PIN_BUTTON_2);
  char cmd = -1;
  // Фиксируем нажатие кнопки
  if( pin_state  == LOW && !button_state_2 && ( ms - ms_button_2 ) > 50 ){
    button_state_2      = true;
    button_long_state_2 = false;
    ms_button_2         = ms;
  }
  // Фиксируем длинное нажатие кнопки
  if( pin_state  == LOW && !button_long_state_2 && ( ms - ms_button_2 ) > 1000 ){
    button_long_state_2 = true;
    // Serial.println("Заблокировать экран");
    cmd = 4;
  }
  // Фиксируем отпускание кнопки
  if( pin_state == HIGH && button_state_2 && ( ms - ms_button_2 ) > 50  ){
    button_state_2      = false;
    ms_button_2         = ms;
    if( !button_long_state_2 ) {
      // Serial.println("Вниз");
      cmd = 1;
    };
  }
  return cmd;
};


display* ds = nullptr;
browser* br = nullptr;
reader* rd = nullptr;
bool state = true; // Режим работы устройства. 1 - браузер, 0 - читалка
bool screenLocked = false;

// unsigned long time = 0;

void(* resetFunc) (void) = 0;
/*
  Функция для перезагрузки контроллера. Сбрасывает счетчик команд на ноль,
  тем самым имитируя перезагрузку контроллера. Все настройки при этом
  остаются теми же самыми. Используется при переходе из режима браузера в
  режим читалки. На этом моменте, судя по всему, кончается память и
  начинается всякая жопа. Поэтому я предпочел просто использовать такой
  костыль. Может быть, исправлю в будущем
*/

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
      ds->setBufSize(bufsize_global);
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
  case 4:
    screenLocked = !screenLocked;
  };
};

void setup() {
  // Serial.begin(9600);
  // pinMode(btn_a, INPUT_PULLUP);
  // pinMode(btn_b, INPUT_PULLUP);
  // pinMode(btn_c, INPUT_PULLUP);

  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);

  if( !SD.begin( card_cs )){
    // Serial.println("init fail");
    return;
  }

  br = new browser();
  ds = new display(bufsize_global, br->buf);
}

void loop() {
  // buttonsHandler(btn());
  buttonsHandler(btn_1_handler());
  buttonsHandler(btn_2_handler());

  if(screenLocked) {
    ds->printEmptyPage();
    return;
  }
  if(state) {
    ds->printDir(br->curfilePosInBuf());
  } else {
    ds->printPage();
  };
}
