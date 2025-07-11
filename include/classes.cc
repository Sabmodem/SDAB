#include "U8glib.h"
#include "rus10x20.h"
#include <Arduino.h>
#include <SD.h>

#define CARD_CS 10 // chip select sd-карты
#define bufsize_reader 4 // chip select sd-карты
#define bufsize_browser 4
#define display_width 127


U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

class display {
private:
  char bufsize;
  String** buf;
public:
  display(char _bufsize, String** _buf) {
    bufsize = _bufsize;
    buf = _buf;
    u8g.setFont(rus10x20); // задаем шрифт
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

  void setBufSize(char _bufsize) {
    bufsize = _bufsize;
  };

  void printPage() { // вывести страницу на экран
    u8g.firstPage();
    do {
      char Y = 15; // позиция Y строки на экране
      u8g.setFont(rus10x20); // задаем шрифт
      for(char curstr = 0; curstr < bufsize; curstr++) { // перебираем буфер
        u8g.setPrintPos(0, Y); // переходим на нужную позицию
        u8g.print(*buf[curstr]); // выводим строку
        Y += 15; // Переходим на новую позицию чтобы не затереть выведенные строки
      };
    } while( u8g.nextPage() );
  };

  void printEmptyPage() { // вывести страницу на экран
    u8g.firstPage();
    do {
      char Y = 15; // позиция Y строки на экране
      u8g.setFont(rus10x20); // задаем шрифт
      for(char curstr = 0; curstr < bufsize; curstr++) { // перебираем буфер
        u8g.setPrintPos(0, Y); // переходим на нужную позицию
        for(char i = 0; i < display_width; i++) {
          u8g.print(" "); // выводим строку
          Y += 15; // Переходим на новую позицию чтобы не затереть выведенные строки
        }
      };
    } while( u8g.nextPage() );
  };

  void printDir(char curpos) {
    *buf[curpos] += "    ###";
    printPage();
    buf[curpos]->remove(buf[curpos]->length() - 7, buf[curpos]->length());
  };
};

class reader {
private:
  File* myFile;
  unsigned char seek; // количество считанных символов
  bool lastMoveDirection; // направление предыдущего чтения. 0 - вперед, 1 - назад
public:
  String* buf[bufsize_reader]; // Буфер дисплея. На этом шрифте помещается 6 строк

  reader(String file) {
    u8g.setFont(rus10x20); // задаем шрифт
    myFile = new File(SD.open(file.c_str()));
    if (!(*myFile)) {
      return;
    };
    fillFirstBuf();
    readPageForward();
    seek = 0;
    lastMoveDirection = false;
  };

  ~reader() {
    clearBuf();
    myFile->close();
    delete myFile;
  };

  void clearBuf() {
    for(char i = 0; i < bufsize_reader; i++) {
      clearString(i);
    };
  };

  void fillFirstBuf() {
    for(char i = 0; i < bufsize_reader; i++) {
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
    for(uint8_t curstr = 0; curstr < bufsize_reader; curstr++) { // заполняем буфер строками
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
    for(int8_t curstr = bufsize_reader-1; curstr >= 0; curstr--) { // заполняем буфер
      clearString(curstr);
      buf[curstr] = readStringBackward();
    };
    lastMoveDirection = true; // записываем направление чтения
  };
};

class browser {
private:
  String* curfile; // указатель на строку с названием текущего файла
public:
  String* buf[bufsize_browser]; // размер буфера вывода в строках
  browser() {
    File* _firstFile = getFirstFile();
    curfile = new String(_firstFile->name());
    _firstFile->close();
    delete _firstFile;
    fillFirstBuf();
  };

  ~browser() {
    clearBuf();
    delete curfile;
    curfile = nullptr;
  };

  File* getFirstFile() { // Возвращает первый файл на диске
    File root = SD.open("/");
    File* result = new File(root.openNextFile());
    root.close();
    return result;
  };

  void fillFirstBuf() { // Заполняет буфер вывода названиями файлов
    File* root = new File(SD.open("/"));
    File* entry;
    for(uint8_t i = 0; i < bufsize_browser; i++) {
      entry = new File(root->openNextFile());
      buf[i] = new String(entry->name());
      entry->close();
      delete entry;
    };
    root->close();
    delete root;
  };

  void clearBuf() {
    for(uint8_t i = 0; i < bufsize_browser; i++) {
      delete buf[i];
      buf[i] = nullptr;
    };
  };

  void moveBufUp() { // Заполняет буфер вывода названиями файлов
    delete buf[0];
    for(uint8_t i = 0; i < bufsize_browser-1; i++) {
      buf[i] = buf[i+1];
    };
    File* _nextFile = nextFile();
    buf[bufsize_browser-1] = new String(_nextFile->name());
    _nextFile->close();
    delete _nextFile;
  };

  void moveBufDown() {
    delete buf[bufsize_browser-1];
    for(uint8_t i = bufsize_browser-1; i > 0; i--) {
      buf[i] = buf[i-1];
    };
    File* _prevFile = prevFile();
    buf[0] = new String(_prevFile->name());
    _prevFile->close();
    delete _prevFile;
  };

  void moveCurfileUp() {
    // Serial.println("moveCurfileUp");
    File* _nextFile = nextFile();
    if(!_nextFile) {
      return;
    };
    if(curfilePosInBuf() == bufsize_browser - 1) {
      moveBufUp();
    };
    delete curfile;
    curfile = new String(_nextFile->name());
    _nextFile->close();
    delete _nextFile;
  };

  void moveCurfileDown() {
    File* _prevFile = prevFile();
    if((curfilePosInBuf() == 0) && strcmp(curfile->c_str(), _prevFile->name()) != 0) {
      moveBufDown();
    };
    *curfile = _prevFile->name();
    _prevFile->close();
    delete _prevFile;
  };

  char curfilePosInBuf() { // Проверяет, существует ли текущий файл в буфере вывода
    char index = -1;
    for(uint8_t i = 0; i < 6; i++) {
      if(*buf[i] == *curfile) {
        index = i;
      };
    };
    return index;
  };

  File* prevFile() { // Получить предыдущий файл. Если файл первый на диске, то возвращает его же
    File root = SD.open("/");
    File entry = root.openNextFile();
    char count = 0;
    File* result = nullptr;
    while ((strcmp(entry.name(), curfile->c_str()) != 0) && (entry)) {
      entry.close();
      entry = root.openNextFile();
      count++;
    };
    entry.close();
    root.rewindDirectory();
    for(char i = 0; i < count-1; i++) {
      entry = root.openNextFile();
      entry.close();
    };
    result = new File(root.openNextFile());
    root.close();
    return result;
  };

  File* nextFile() { // Получить следующий файл
    File root = SD.open("/");
    File entry;
    String name;
    File resultFile;
    File* resultPointer = nullptr;
    do {
      entry = root.openNextFile();
      name = entry.name();
      entry.close();
    } while(strcmp(name.c_str(), curfile->c_str()) != 0);
    resultFile = root.openNextFile();
    if(resultFile) {
      resultPointer = new File(resultFile);
    };
    root.close();
    return resultPointer;
  };

  String* getCurfile() { // Получить контекст текущего файла
    return curfile;
  };
};
