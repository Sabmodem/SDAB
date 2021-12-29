#include "U8glib.h"
#include <SD.h>
#include "rus6x10.h"

#define CARD_CS 10 // chip select sd-карты
#define bufsize 6 // chip select sd-карты

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

class reader {
private:
  File myFile;
  String buf[bufsize]; // Буфер дисплея. На этом шрифте помещается 6 строк
  unsigned int seek; // количество считанных символов
  bool lastMoveDirection; // направление предыдущего чтения. 0 - вперед, 1 - назад
public:
  reader(String file) {
    u8g.setFont(rus6x10); // задаем шрифт

    Serial.begin(9600);
    Serial.println("Initializing SD card...");

    if( !SD.begin( CARD_CS )){ // включаем sd-карту
      Serial.println("initialization failed!");
      return;
    };

    myFile = SD.open(file.c_str());
    if (!myFile) {
      Serial.println("file open failed!");
      return;
    };
    seek = 0;
    lastMoveDirection = 0;
  };

  ~reader() {
    clearBuf();
    myFile.close();
  };

  void clearBuf() {
    for(char i = 0; i < bufsize; i++) {
      buf[i] = "";
    };
  };

  char findStingInPage(String str) {
    // Serial.println("target: ");
    // Serial.println(str);
    char index = -1;
    for(uint8_t i = 0; i < 6; i++) {
      // Serial.print("tmp: ");
      // Serial.print(i);
      // Serial.print(" : ");
      // Serial.println(page[i]);
      if(buf[i] == str) {
        index = i;
        // break;
      };
    };
    return index;
  };

  String readStringBackward() { // Считать строку назад
    String str = ""; // строка
    while(u8g.getStrWidth(str.c_str()) < 128) { // Пока считанная строка помещается в экран
      if(myFile.position() == 0) { // Если уперлись в начало файла
        seek = 0; // то сбрасываем счетчик
        break; // и выходим из цикла
      };
      str = (char)myFile.peek() + str; // Переворачиваем строку, иначе будет строка наоборот
      myFile.seek(myFile.position()-1); // движемся назад по файлу
    };
    if(u8g.getStrWidth(str.c_str()) > 128) { // если случайно попал лишний символ
      str.remove(str.length()-1); // то убираем его
      myFile.seek(myFile.position()+1); // корректируем положение в файле
    };
    seek += str.length(); // считаем символы
    return str;
  };

  String readStringForward() { // считать строку вперед
    String str = ""; // строка
    while(u8g.getStrWidth(str.c_str()) < 128) { // пока строка не вылазит за пределы экрана
      if(!myFile.available()) { // Если уперлись в конец файла
        break; // то выходим из цикла
      };
      str += (char)myFile.read(); // записываем символ в строку
    };
    if(u8g.getStrWidth(str.c_str()) > 128) { // если попал лишний символ
      str.remove(str.length()-1); // то убираем его
      myFile.seek(myFile.position()-1); // и корректируем положение в файле
    };
    seek += str.length(); // считаем символы
    return str;
  };

  void readPageForward() { // считать страницу вперед
    if(!myFile.available()) { // Если уперлись в конец файла
      return; // то ничего не делаем
    };
    if(lastMoveDirection) { // Если предыдущее чтение было назад
      myFile.seek(myFile.position() + seek);
      /* Cчитаем, на каком положении должна стоять каретка и переходим туда.
         seek - счетчик считанных символов. lastMoveDirection - направление
         предыдущего чтения. Без них приходится 2 раза нажимать кнопку чтобы
         страница считалась. Когда страница считана, каретка стоит на последнем
         считанном символе. Получается, что если читать в обратном направлении,
         придется считать только что считанную страницу и только потом будет
         предыдущая. Чтобы такого не было я ввел эти переменные
      */
    };
    seek = 0; // сбрасываем счечик для следующего чтения
    for(uint8_t curstr = 0; curstr < 6; curstr++) { // заполняем буфер строками
      buf[curstr] = readStringForward(); // читаем строку и записываем в буфер
    };
    lastMoveDirection = 0; // записываем направление чтения
  };

  void readPageBackward() { // считать страницу назад
    if(myFile.position() == 0) { // Если уперлись в конец файла
      return; // то ничего не делаем
    };
    if(!lastMoveDirection) { // если последнее чтение было вперед
      myFile.seek(myFile.position() - seek); // корректируем позицию в файле аналогично методу выше
    };
    seek = 0; // сбрасываем счетчик
    for(int8_t curstr = 5; curstr >= 0; curstr--) { // заполняем буфер
      // Аналогично методу выше, только в обратном направлении
      buf[curstr] = readStringBackward();
    };
    lastMoveDirection = 1; // записываем направление чтения
  };

  // void print() { // вывести страницу на экран
  //   uint8_t Y = 10; // позиция Y строки на экране
  //   u8g.setFont(rus6x10); // задаем шрифт
  //   for(uint8_t curstr = 0; curstr < 6; curstr++) { // перебираем буфер
  //     u8g.setPrintPos(0, Y); // переходим на нужную позицию
  //     u8g.print(page[curstr]); // выводим строку
  //     Y += 10; // Переходим на новую позицию чтобы не затереть выведенные строки
  //   };
  // }

  void print() { // вывести страницу на экран
    for(char i = 0; i < bufsize; i++) {
      Serial.print(i, DEC);
      Serial.print(" : ");
      Serial.println(buf[i]);
    };
    Serial.println();
  };
};
