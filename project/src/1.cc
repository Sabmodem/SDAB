#include "U8glib.h"
#include "rus6x10.h"

#include <Arduino.h>
#include <SD.h>
#include "browser.h"
#include "browser.cc"


// void print(String* buf) { // вывести страницу на экран
//   char Y = 10; // позиция Y строки на экране
//   u8g.setFont(rus6x10); // задаем шрифт
//   for(char curstr = 0; curstr < bufsize; curstr++) { // перебираем буфер
//     u8g.setPrintPos(0, Y); // переходим на нужную позицию
//     // Serial.println(buf[curstr]);
//     u8g.print(buf[curstr]); // выводим строку
//     Y += 10; // Переходим на новую позицию чтобы не затереть выведенные строки
//   };
// };

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

void buttonsHandler(char cmd, browser* br) {
  switch (cmd) {
  case 'n': // Если нажата n
    // rd->readPageForward(); // читаем вперед
    // rd->print();
    br->moveCurfileUp(); // читаем вперед
    br->print();
    break;
  case 'p': // если нажата p
    // rd->readPageBackward(); // читаем назад
    // rd->print();
    br->moveCurfileDown(); // читаем вперед
    br->print();
    break;
  case 'w': // если нажата p
    // rd->readPageBackward(); // читаем назад
    // rd->print();
    // U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI
    break;
  };
};

// String buf[6];
// browser* br = nullptr;
// display* ds;



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

  // br = new browser();
  browser br = browser();
  for(char i = 0; i < 5; i++) {
    br.moveCurfileUp();
    br.print();
  };
  // ds = new display(6);

  // br->moveCurfileUp();
  // br->print();

  // String* buf = br->getBuf();

  // ds->print(buf);

  // for(char i = 0; i < 6; i++) {
  //   // Serial.println(buf[i]);
  //   buff[i] = "##########";
  // };
}

void loop() {
  // u8g.firstPage();
  // // цикл вывода. Выводит страницами чтобы экономить память
  // // НЕ ТРОГАТЬ, ИНАЧЕ СЛОМАЕТСЯ
  // do {
  //   // print(buff);
  // } while( u8g.nextPage() );
  // // ds->preparePages(buf);
  // // ds.preparePages(buf);
  // if(Serial.available()) {
  //   buttonsHandler(Serial.read(), br);
  // };
};
