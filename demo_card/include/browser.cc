#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

void browser::fileContext::print() const {
  Serial.print("root: ");
  Serial.print(root.name());
  Serial.print(" ; file: ");
  Serial.println(curfile.name());
};

void browser::fileContext::close() {
  curfile.close();
  root.close();
};

browser::browser() {
  Serial.println("constructor");
  File _firstFile = getFirstFile();
  curfile = new String(_firstFile.name());
  fillFirstBuf();
  print();
  _firstFile.close();
};

browser::~browser() {
  Serial.println("destructor");
  clearBuf();
  delete curfile;
  curfile = nullptr;
};

void browser::print() { // Напечатать буфер вывода в serial
  char _curfilePosInBuf = curfilePosInBuf();
  String* _curfile = nullptr;
  if(_curfilePosInBuf != -1) {
    _curfile = &buf[_curfilePosInBuf];
    *_curfile += "    ###";
  };
  Serial.println("FILES:");
  for(uint8_t i = 0; i < bufsize; i++) {
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(buf[i]);
  };
  if(_curfilePosInBuf != -1) {
    _curfile->remove(_curfile->length() - 7, _curfile->length());
  };

  Serial.print("cur file: ");
  Serial.println(*curfile);
  Serial.print("next file: ");
  File _nextFile = nextFile();
  Serial.println(_nextFile.name());
  _nextFile.close();
  Serial.print("prev file: ");
  File _prevFile = prevFile();
  Serial.println(_prevFile.name());
  _prevFile.close();
  Serial.print("curfile context: ");
  fileContext ctx = curfileContext();
  ctx.print();
  ctx.close();
};

// void browser::printO() {
//   for(uint8_t i = 0; i < bufsize; i++) {
//     Serial.print(i);
//     Serial.print(" : ");
//     Serial.println(buf[i]);
//   };
// };

void browser::printO() { // вывести страницу на экран
  uint8_t Y = 10; // позиция Y строки на экране
  u8g.setFont(rus6x10); // задаем шрифт
  for(uint8_t curstr = 0; curstr < 6; curstr++) { // перебираем буфер
    u8g.setPrintPos(0, Y); // переходим на нужную позицию
    u8g.print(buf[curstr]); // выводим строку
    Y += 10; // Переходим на новую позицию чтобы не затереть выведенные строки
  };
}


File browser::getFirstFile() { // Возвращает первый файл на диске
  File root = SD.open("/");
  File result = root.openNextFile();
  root.close();
  return result;
};

void browser::fillFirstBuf() { // Заполняет буфер вывода названиями файлов
  fileContext _curfileContext = curfileContext();
  for(uint8_t i = 0; i < bufsize; i++) {
    buf[i] = String(_curfileContext.curfile.name());
    _curfileContext.curfile.close();
    _curfileContext.curfile = _curfileContext.root.openNextFile();
  };
  _curfileContext.close();
};

void browser::clearBuf() {
  for(uint8_t i = 0; i < bufsize; i++) {
    buf[i] = "";
  };
};

void browser::moveBufUp() { // Заполняет буфер вывода названиями файлов
  for(uint8_t i = 0; i < bufsize-1; i++) {
    buf[i] = buf[i+1];
  };
  File _nextFile = nextFile();
  buf[bufsize-1] = _nextFile.name();
  _nextFile.close();
};

void browser::moveBufDown() {
  for(uint8_t i = bufsize-1; i > 0; i--) {
    buf[i] = buf[i-1];
  };
  File _prevFile = prevFile();
  buf[0] = _prevFile.name();
  _prevFile.close();
};

void browser::moveCurfileUp() {
  File _nextFile = nextFile();
  if(!_nextFile) {
    _nextFile.close();
    return;
  };
  if(curfilePosInBuf() == bufsize - 1) {
    moveBufUp();
  };
  *curfile = _nextFile.name();
  _nextFile.close();
};

void browser::moveCurfileDown() {
  File _prevFile = prevFile();
  if(strcmp(_prevFile.name(), curfile->c_str()) == 0) {
    _prevFile.close();
    return;
  };
  if(curfilePosInBuf() == 0) {
    moveBufDown();
  };
  *curfile = _prevFile.name();
  _prevFile.close();
};

char browser::curfilePosInBuf() { // Проверяет, существует ли текущий файл в буфере вывода
  char index = -1;
  for(uint8_t i = 0; i < 6; i++) {
    if(buf[i] == *curfile) {
      index = i;
    };
  };
  return index;
};

File browser::prevFile() { // Получить предыдущий файл. Если файл первый на диске, то возвращает его же
  File root = SD.open("/");
  File entry = root.openNextFile();
  File result;
  char count = 0;
  while (!(String(entry.name()) == *curfile || !entry)) {
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
  result = root.openNextFile();
  root.close();
  return result;
};

File browser::nextFile() { // Получить следующий файл
  File root = SD.open("/");
  File entry;
  String name;
  File result;
  do {
    entry = root.openNextFile();
    name = entry.name();
    entry.close();
  } while(name != *curfile);
  result = root.openNextFile();
  root.close();
  return result;
};

browser::fileContext browser::curfileContext() { // Получить контекст текущего файла
  fileContext ctx;
  File root = SD.open("/");
  File _prevFile = prevFile();
  ctx.root = root;
  if(*curfile == String(_prevFile.name())) {
    root.openNextFile().close();
    ctx.curfile = _prevFile;
    return ctx;
  };
  File entry;
  String name;
  File result;
  do {
    entry = root.openNextFile();
    name = String(entry.name());
    entry.close();
  } while(name != _prevFile.name() && name.length() != 0);
  _prevFile.close();
  result = root.openNextFile();
  ctx.curfile = result;
  return ctx;
};

String* browser::getCurfile() { // Получить контекст текущего файла
  return curfile;
};
String* browser::getBuf() { // Получить контекст текущего файла
  return buf;
};
