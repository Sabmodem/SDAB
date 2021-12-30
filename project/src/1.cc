#include "U8glib.h"
#include "rus6x10.h"

#include <Arduino.h>
#include <SD.h>
#include "browser.cc"

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

browser* br = nullptr;
char pos;

void print() { // вывести страницу на экран
  pos = br->curfilePosInBuf();
  br->bufBeforePrint();
  char Y = 10; // позиция Y строки на экране
  u8g.setFont(rus6x10); // задаем шрифт
  for(char curstr = 0; curstr < 6; curstr++) { // перебираем буфер
    u8g.setPrintPos(0, Y); // переходим на нужную позицию
    // Serial.println(buf[curstr]);
    u8g.print(*br->buf[curstr]); // выводим строку
    Y += 10; // Переходим на новую позицию чтобы не затереть выведенные строки
  };
  br->bufAfterPrint(pos);
};

void buttonsHandler(char cmd, browser* br) {
  // char _curfilePosInBuf = br->curfilePosInBuf();
  switch (cmd) {
  case 'n': // Если нажата n
    br->moveCurfileUp(); // читаем вперед
    br->print();
    // buf = br->getBuf();
    break;
  case 'p': // если нажата p
    br->moveCurfileDown(); // читаем вперед
    br->print();
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

  // u8g.setFont(rus6x10); // задаем шрифт
  // if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
  //   u8g.setColorIndex(255);     // white
  // }
  // else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
  //   u8g.setColorIndex(3);         // max intensity
  // }
  // else if ( u8g.getMode() == U8G_MODE_BW ) {
  //   u8g.setColorIndex(1);         // pixel on
  // }
  // else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
  //   u8g.setHiColorByRGB(255,255,255);
  // }

  br = new browser();
}

void loop() {
  if(Serial.available()) {
    buttonsHandler(Serial.read(), br);
  };

  u8g.firstPage();
  // цикл вывода. Выводит страницами чтобы экономить память
  // НЕ ТРОГАТЬ, ИНАЧЕ СЛОМАЕТСЯ
  do {
    print();
  } while( u8g.nextPage() );
};
