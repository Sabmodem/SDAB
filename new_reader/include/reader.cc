#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "U8glib.h"
#include "rus6x10.h"

#define CARD_CS 10 // chip select sd-карты
#define bufsize 3 // chip select sd-карты
#define display_width 128

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

class reader {
private:
  File* myFile;
  // String buf[bufsize]; // Буфер дисплея. На этом шрифте помещается 6 строк
  unsigned char seek; // количество считанных символов
  bool lastMoveDirection; // направление предыдущего чтения. 0 - вперед, 1 - назад
public:
  String* buf[bufsize]; // Буфер дисплея. На этом шрифте помещается 6 строк

  reader(String file) {
    u8g.setFont(rus6x10); // задаем шрифт

    Serial.begin(9600);
    Serial.println("Initializing SD card...");

    myFile = new File(SD.open(file.c_str()));
    if (!(*myFile)) {
      Serial.println("file open failed!");
      return;
    };
    fillFirstBuf();
    seek = 0;
    lastMoveDirection = false;
  };

  ~reader() {
    Serial.println("~reader");
    clearBuf();
    myFile->close();
    delete myFile;
  };

  void clearBuf() {
    for(char i = 0; i < bufsize; i++) {
      clearString(i);
    };
  };

  void fillFirstBuf() {
    for(char i = 0; i < bufsize; i++) {
      buf[i] = new String();
    };
  };

  void clearString(char num) {
    if(buf[num] != nullptr && *buf[num]) {
      delete buf[num];
      buf[num] = nullptr;
    };
  };

  String* readStringForward() { // считать строку вперед
    String* str = new String(); // строка
    while((u8g.getStrWidth(str->c_str()) < display_width - 1) && (myFile->available())) { // пока строка не вылазит за пределы экрана
      *str += (char)myFile->read(); // записываем символ в строку
    };
    seek += str->length(); // считаем символы
    return str;
  };

  void readPageForward() { // считать страницу вперед
    if(lastMoveDirection) { // Если предыдущее чтение было назад
      myFile->seek(myFile->position() + seek);
      //   /* Cчитаем, на каком положении должна стоять каретка и переходим туда.
      //      seek - счетчик считанных символов. lastMoveDirection - направление
      //      предыдущего чтения. Без них приходится 2 раза нажимать кнопку чтобы
      //      страница считалась. Когда страница считана, каретка стоит на последнем
      //      считанном символе. Получается, что если читать в обратном направлении,
      //      придется считать только что считанную страницу и только потом будет
      //      предыдущая. Чтобы такого не было я ввел эти переменные
      //   */
    };
    seek = 0; // сбрасываем счечик для следующего чтения
    for(uint8_t curstr = 0; curstr < bufsize; curstr++) { // заполняем буфер строками
      clearString(curstr);
      buf[curstr] = readStringForward(); // читаем строку и записываем в буфер
    };
    lastMoveDirection = false; // записываем направление чтения
  };

  String* readStringBackward() { // Считать строку назад
    String* str = new String(); // строка
    while((u8g.getStrWidth(str->c_str()) < display_width - 1) && (myFile->position())) { // Пока считанная строка помещается в экран
      *str = (char)myFile->peek() + *str; // Переворачиваем строку, иначе будет строка наоборот
      myFile->seek(myFile->position()-1); // движемся назад по файлу
    };
    seek += str->length(); // считаем символы
    return str;
  };

  void readPageBackward() { // считать страницу назад
    if(!lastMoveDirection) { // если последнее чтение было вперед
      myFile->seek(myFile->position() - seek); // корректируем позицию в файле аналогично методу выше
    };
    seek = 0; // сбрасываем счетчик
    for(int8_t curstr = bufsize-1; curstr >= 0; curstr--) { // заполняем буфер
      clearString(curstr);
      buf[curstr] = readStringBackward();
    };
    lastMoveDirection = true; // записываем направление чтения
  };

  void print() { // вывести страницу на экран
    for(char i = 0; i < bufsize; i++) {
      Serial.print(i, DEC);
      Serial.print(" : ");
      Serial.print(*buf[i]);
      Serial.print(" ; length: ");
      Serial.println(buf[i]->length());
    };
    // Serial.println();
    // Serial.print("seek: ");
    // Serial.print(seek, DEC);
    // Serial.print(" ;lastMoveDirection: ");
    // Serial.print(lastMoveDirection);
    // Serial.print(" ;position: ");
    // Serial.print(myFile->position());
    // Serial.print(" ;available: ");
    // Serial.print(myFile->available());
    // Serial.print(" ;File(bool): ");
    // Serial.println((bool)(*myFile));
    Serial.println();
  };
};
