#include "U8glib.h"
#include <SD.h>
#include <Arduino.h>
#include "rus6x10.h"

static const char CARD_CS = 10; // chip select sd-карты
static const char btn_a = 3;
static const char btn_b = 4;
unsigned long time = 0;

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI

File myFile;
String page[6]; // Буфер дисплея. На этом шрифте помещается 6 строк
unsigned int seek = 0; // количество считанных символов
bool lastMoveDirection = 0; // направление предыдущего чтения. 0 - вперед, 1 - назад
String* curfile = nullptr;
char countSteps = 0; // Номер текущего файла
// String curfile = "";
char countFiles = 0;
char curfileIndex = 0;
// char filesBottomLine = 0;
// char filesUpperLine = 6;
bool state = false; // Режим работы устройства. 0 - обозреватель файлов, 1 - читалка

char getCountFiles() {
  File root = SD.open("/");
  File entry;
  char countFiles = 0;
  while(1) {
    entry = root.openNextFile();
    if(!entry) {
      break;
    };
    // Serial.println(entry.name());
    countFiles += 1;
    entry.close();
  };
  root.close();
  // Serial.print("count files: ");
  // Serial.println(countFiles, DEC);
  return countFiles;
};

// void listDir1(char marker=0) { // НЕ ОПРОБОВАНО
//   Serial.println("listDir");
//   char countFiles = 0;
//   File entry;
//   File root = SD.open("/");
//   char indexFile = 0;
//   for(int8_t curfile = 0; curfile < filesBottomLine; curfile++) {
//     entry = root.openNextFile();
//     entry.close();
//   };
//   for(int8_t curfile = filesBottomLine; curfile < filesUpperLine; curfile++) { // заполняем буфер
//     entry = root.openNextFile();
//     if (!entry) {
//       // файлов больше нет
//       for(int8_t i = indexFile; i < 6; i++) {
//         page[i] = "";
//       };
//       break;
//     }
//     Serial.println(entry.name());
//     page[indexFile] = entry.name();
//     indexFile += 1;
//     entry.close();
//   };
//   root.close();
//   curfile = &page[marker];
//   *curfile += "    ***";
// };

void listDir(char pos=0, char marker=0) { // НЕ ОПРОБОВАНО
  if(pos > countFiles - 6) {
    marker = countFiles - pos;
    pos = countFiles - 6;
  };
  if(pos < 6) {
    pos = 0;
  };
  Serial.println("listDir");
  Serial.print("pos: ");
  Serial.print(pos, DEC);
  Serial.print(" ; marker: ");
  Serial.println(marker, DEC);

  File root = SD.open("/");
  char indexFile = 0;
  File entry;

  for(int8_t curfile = 0; curfile < pos; curfile++) {
    if(!root) {
      Serial.println("wrong root");
    };
    entry = root.openNextFile();
    if (!entry) {
      Serial.println("wrong entry");
    };
    Serial.print("tmp: ");
    Serial.print(entry.name());
    Serial.print(" ; root: ");
    Serial.println(root.name());
    entry.close();
  };

  for(int8_t curfile = pos; curfile < pos+6; curfile++) { // заполняем буфер
    if(!root) {
      Serial.println("wrong root");
    };
    entry = root.openNextFile();
    if (!entry) {
      Serial.println("wrong entry");
      // файлов больше нет
      for(int8_t i = indexFile; i < 6; i++) {
        page[i] = "";
      };
      break;
    }
    // Serial.println(entry.name());
    page[indexFile] = entry.name();
    Serial.print("use: ");
    Serial.print(entry.name());
    Serial.print(" ; root: ");
    Serial.println(root.name());
    // Serial.print(" ; root (HEX): ");
    // Serial.println(root->name(), HEX);
    entry.close();
    indexFile++;
  };

  root.close();
  curfile = &page[marker];
  *curfile += "    ***";
};


// void moveFilesFrameForward() {
//   Serial.print("moveFilesFrameForward ; condition: ");
//   Serial.println(filesUpperLine < countFiles);
//   if(filesUpperLine < countFiles) {
//     filesUpperLine += 6;
//     filesBottomLine += 6;
//     if(filesUpperLine > countFiles) {
//       filesUpperLine = countFiles;
//     };
//   };
//   Serial.print("filesUpperLine: ");
//   Serial.print(filesUpperLine, DEC);
//   Serial.print(" ; filesBottomLine: ");
//   Serial.println(filesBottomLine, DEC);
// };

// void moveFilesFrameBackward() {
//   if(filesBottomLine > 0) {
//     filesUpperLine -= 6;
//     filesBottomLine -= 6;
//     if(filesBottomLine < 0) {
//       // filesUpperLine -= 6;
//       filesBottomLine = 0;
//     };
//   };
//   Serial.print("filesUpperLine: ");
//   Serial.print(filesUpperLine, DEC);
//   Serial.print("; filesBottomLine: ");
//   Serial.print(filesBottomLine, DEC);
// };

void serialPage() {
  for(uint8_t i = 0; i < 6; i++) {
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(page[i]);
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
    if(page[i] == str) {
      index = i;
      // break;
    };
  };
  return index;
};

void moveCurfileForward() {
  // if(countSteps == countFiles) {
  //   return;
  // };
  curfile->remove(curfile->length() - 7, curfile->length());
  File root = SD.open("/");
  File entry = root.openNextFile();
  while (!(String(entry.name()) == *curfile || !entry)) {
    entry.close();
    entry = root.openNextFile();
  };
  entry.close();
  File nextFile = root.openNextFile();
  if(!root) {
    Serial.println("wrong root");
  };
  if(!nextFile) {
    Serial.println("wrong entry");
    // *curfile += "    ***";
    return;
  };
  *curfile = nextFile.name();
  // char fileIndex = findStingInPage(nextFile.name());
  // nextFile.close();
  // root.close();
  // if(fileIndex != -1) {
  //   curfile = &page[fileIndex];
  //   page[fileIndex] += "    ***";
  // } else {
  //   // moveFilesFrameForward();
  //   listDir(countSteps+1);
  // };
  countSteps++;
  Serial.print("count steps: ");
  Serial.print(countSteps, DEC);
  Serial.print(" ;curfile: ");
  Serial.println(nextFile.name());
  root.close();
};

void moveCurfileBackward() {
  if(countSteps == 0) {
    return;
  };
  Serial.println("moveCurfileBackward");
  curfile->remove(curfile->length() - 7, curfile->length());
  File root = SD.open("/");
  File entry;
  for(char i = 0; i < countSteps - 1; i++){
    entry = root.openNextFile();
    entry.close();
  };
  entry = root.openNextFile();
  // char fileIndex = findStingInPage(entry.name());
  // entry.close();
  // if(fileIndex != -1) {
  //   curfile = &page[fileIndex];
  //   page[fileIndex] += "    ***";
  // } else {
  //   // moveFilesFrameBackward();
  //   listDir(countSteps-6, 5);
  // };
  countSteps--;
  Serial.print("count steps: ");
  Serial.print(countSteps, DEC);
  Serial.print(" ;curfile: ");
  Serial.println(entry.name());
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
    page[curstr] = readStringForward(); // читаем строку и записываем в буфер
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
    page[curstr] = readStringBackward();
  };
  lastMoveDirection = 1; // записываем направление чтения
};

void printPage() { // вывести страницу на экран
  uint8_t Y = 10; // позиция Y строки на экране
  u8g.setFont(rus6x10); // задаем шрифт
  for(uint8_t curstr = 0; curstr < 6; curstr++) { // перебираем буфер
    u8g.setPrintPos(0, Y); // переходим на нужную позицию
    u8g.print(page[curstr]); // выводим строку
    Y += 10; // Переходим на новую позицию чтобы не затереть выведенные строки
  };
}

char btn() {
  if(millis() - time < 1000) {
    return 0;
  };
  char btn_a_state = digitalRead(btn_a);
  char btn_b_state = digitalRead(btn_b);
  char state = 0;
  if(btn_a_state == LOW && btn_b_state == LOW) {
    state = 0;
  } else if(btn_a_state == HIGH && btn_b_state == LOW) {
    Serial.println("Вверх");
    state = 1;
  } else if(btn_a_state == LOW && btn_b_state == HIGH) {
    Serial.println("Вниз");
    state = 2;
  }
  time = millis();
  return state;
}

void mainExecReader(char cmd) { // имитация нажатия кнопок
  switch (cmd) {
  // case 'f': // Если нажата f
  case 1: // Если нажата f
    readPageForward(); // читаем вперед
    break;
  // case 'b': // если нажата b
  case 2: // если нажата b
    readPageBackward(); // читаем назад
    break;
  // case 'l': // вывод отладочной информации
  //   Serial.print("seek: ");
  //   Serial.print(seek);
  //   Serial.print(", pos: ");
  //   Serial.print(myFile.position());
  //   Serial.print(", lmd: ");
  //   Serial.print(lastMoveDirection);
  //   Serial.print(", avl: ");
  //   Serial.println(myFile.available());
  };
};

void mainExecBrowser(char cmd) { // имитация нажатия кнопок
  // Serial.println(cmd, DEC);
  switch (cmd) {
    // case 'f': // Если нажата f
  case 1: // Если нажата f
    moveCurfileForward();
    break;
  case 2: // если нажата b
    moveCurfileBackward();
    // readPageBackward(); // читаем назад
    break;
    // case 'l': // вывод отладочной информации
    //   Serial.print("seek: ");
    //   Serial.print(seek);
    //   Serial.print(", pos: ");
    //   Serial.print(myFile.position());
    //   Serial.print(", lmd: ");
    //   Serial.print(lastMoveDirection);
    //   Serial.print(", avl: ");
    //   Serial.println(myFile.available());
  };
};

void mainExec() {
  switch (state) {
  case false:
    mainExecBrowser(btn());
    break;
  case true:
    mainExecReader(btn());
    break;
  }
};

void setup(void) {
  pinMode(btn_a, INPUT_PULLUP);
  pinMode(btn_b, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("\nInitializing SD card...");

  u8g.setFont(rus6x10); // задаем шрифт

  if( !SD.begin( CARD_CS )){ // включаем sd-карту
    Serial.println("initialization failed!");
    return;
  };

  // myFile = SD.open("test1.txt"); // открываем файл
  // if (!myFile) {
  //   Serial.println("file open failed!");
  //   return;
  // };
  // myFile = SD.open("/"); // открываем файл
  // if (!myFile) {
  //   Serial.println("file open failed!");
  //   return;
  // };
  countFiles = getCountFiles();
  listDir();


  // flip screen, if required
  // u8g.setRot180();

  // set SPI backup if required
  // u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  // тут всякие настройки экрана
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
}

void loop(void) {
  // if(Serial.available() > 0) {
  //   mainExec(Serial.read()); // обрабатываем нажатия кнопок
  // };
  mainExec();
  // btn();
  // if(seek == 0) {
  //   if(myFile.position() == 0 && myFile.available()) {
  //     readPageForward();
  //     // вывод первой страницы. Не дает вылезти за границу буфера
  //   };
  // };

  u8g.firstPage();
  // цикл вывода. Выводит страницами чтобы экономить память
  // НЕ ТРОГАТЬ, ИНАЧЕ СЛОМАЕТСЯ
  do {
    printPage();
  } while( u8g.nextPage() );
}
