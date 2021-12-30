#include <Arduino.h>

// #define bufsize 6 // chip select sd-карты

class display {
private:
  char bufsize;
public:
  display(char _bufsize) {
    // U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);
    bufsize = _bufsize;
    // u8g(U8G_I2C_OPT_NONE);
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

  void print(String* buf) { // вывести страницу на экран
    char Y = 10; // позиция Y строки на экране
    u8g.setFont(rus6x10); // задаем шрифт
    for(char curstr = 0; curstr < bufsize; curstr++) { // перебираем буфер
      u8g.setPrintPos(0, Y); // переходим на нужную позицию
      Serial.println(buf[curstr]);
      u8g.print(buf[curstr]); // выводим строку
      Y += 10; // Переходим на новую позицию чтобы не затереть выведенные строки
    };
  };

  void preparePages(String* buf) {
    u8g.firstPage();
    // цикл вывода. Выводит страницами чтобы экономить память
    // НЕ ТРОГАТЬ, ИНАЧЕ СЛОМАЕТСЯ
    do {
      print(buf);
    } while( u8g.nextPage() );
  };
};
