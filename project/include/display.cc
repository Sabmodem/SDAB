#include "U8glib.h"
#include "rus6x10.h"
#include <Arduino.h>

// #define bufsize 6 // chip select sd-карты
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

class display {
private:
  char bufsize;
  String** buf;
public:
  display(char _bufsize, String** _buf) {

    bufsize = _bufsize;
    buf = _buf;
    u8g.setFont(rus6x10); // задаем шрифт
    if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
      u8g.setColorIndex(255);     // white
    }
    else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
      u8g.setColorIndex(3);         // max intensity
    }
    else if ( u8g.getMode() == U8G_MODE_BW ) {
      u8g.setColorIndex(1);         // pixel on
    }
    else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
      u8g.setHiColorByRGB(255,255,255);
    }
  };

  void setBufPtr(String** _buf) {
    buf = _buf;
  };

  void printPage() { // вывести страницу на экран
    u8g.firstPage();
    do {
      char Y = 10; // позиция Y строки на экране
      u8g.setFont(rus6x10); // задаем шрифт
      for(char curstr = 0; curstr < bufsize; curstr++) { // перебираем буфер
        u8g.setPrintPos(0, Y); // переходим на нужную позицию
        // Serial.println(*buf[curstr]);
        u8g.print(*buf[curstr]); // выводим строку
        Y += 10; // Переходим на новую позицию чтобы не затереть выведенные строки
      };
    } while( u8g.nextPage() );
  };

  void printDir(char curpos) {
    *buf[curpos] += "    ###";
    printPage();
    buf[curpos]->remove(buf[curpos]->length() - 7, buf[curpos]->length());
  };
};
