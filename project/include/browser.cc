#define bufsize      6

class browser {
private:
  String* curfile; // указатель на строку с названием текущего файла
public:
  String* buf[bufsize]; // размер буфера вывода в строках
  browser() {
    // Serial.println("constructor");
    // buf = new String[bufsize];
    File* _firstFile = getFirstFile();
    curfile = new String(_firstFile->name());
    _firstFile->close();
    delete _firstFile;
    fillFirstBuf();
    print();
  };

  ~browser() {
    // Serial.println("destructor");
    clearBuf();
    delete curfile;
    curfile = nullptr;
  };

  void print() { // Напечатать буфер вывода в serial
    char _curfilePosInBuf = curfilePosInBuf();
    String* _curfile = nullptr;
    if(_curfilePosInBuf != -1) {
      _curfile = buf[_curfilePosInBuf];
      *_curfile += "    ###";
    };
    Serial.println("FILES:");
    for(uint8_t i = 0; i < bufsize; i++) {
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(*buf[i]);
    };
    if(_curfilePosInBuf != -1) {
      _curfile->remove(_curfile->length() - 7, _curfile->length());
    };

    Serial.print("CURRENT FILE: ");
    Serial.println(*curfile);

    File* _nextFile = nextFile();
    if(_nextFile)  {
      Serial.print("NEXT FILE: ");
      Serial.print(_nextFile->name());
      Serial.print(" NEXT FILE(BOOL): ");
      Serial.println((bool)_nextFile);
      _nextFile->close();
      delete _nextFile;
    } else {
      Serial.println("nextfile is null");
    };

    File* _prevFile = prevFile();
    Serial.print("PREV FILE: ");
    Serial.print(_prevFile->name());
    Serial.print(" PREV FILE(BOOL): ");
    Serial.println((bool)_prevFile);
    _prevFile->close();
    delete _prevFile;

    // fileContext* ctx = curfileContext();
    // Serial.print("CUR FILE CTX: ");
    // ctx->print();
    // ctx->close();
    // delete ctx;
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
    for(uint8_t i = 0; i < bufsize; i++) {
      entry = new File(root->openNextFile());
      buf[i] = new String(entry->name());
      entry->close();
      delete entry;
    };
    root->close();
    delete root;
  };

  void clearBuf() {
    return;
    // for(uint8_t i = 0; i < bufsize; i++) {
      // buf[i] = "";
    // };
  };

  void moveBufUp() { // Заполняет буфер вывода названиями файлов
    delete buf[0];
    for(uint8_t i = 0; i < bufsize-1; i++) {
      buf[i] = buf[i+1];
    };
    File* _nextFile = nextFile();
    buf[bufsize-1] = new String(_nextFile->name());
    _nextFile->close();
    delete _nextFile;
  };

  void moveBufDown() {
    delete buf[bufsize-1];
    for(uint8_t i = bufsize-1; i > 0; i--) {
      buf[i] = buf[i-1];
    };
    File* _prevFile = prevFile();
    buf[0] = new String(_prevFile->name());
    _prevFile->close();
    delete _prevFile;
  };

  void moveCurfileUp() {
    Serial.println("moveCurfileUp");
    File* _nextFile = nextFile();
    if(!_nextFile) {
      return;
    };
    if(curfilePosInBuf() == bufsize - 1) {
      moveBufUp();
    };
    delete curfile;
    curfile = new String(_nextFile->name());
    _nextFile->close();
    delete _nextFile;
  };

  void moveCurfileDown() {
    File* _prevFile = prevFile();
    // if(strcmp(_prevFile->name(), curfile->c_str()) == 0) {
    //   _prevFile->close();
    //   delete _prevFile;
    //   return;
    // };
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
    // Serial.print("curfile name: ");
    // Serial.println(*curfile);
    while ((strcmp(entry.name(), curfile->c_str()) != 0) && (entry)) {
      entry.close();
      entry = root.openNextFile();
      // Serial.print("entry name: ");
      // Serial.print(entry.name());
      // Serial.print(" ; curfile name: ");
      // Serial.print(*curfile);
      // Serial.print(" ; condition: ");
      // Serial.println(String(entry.name()) != *curfile && entry);
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
    // File* result = nullptr;

    File resultFile;
    File* resultPointer = nullptr;

    // Serial.print("curfile name: ");
    // Serial.println(*curfile);
    do {
      entry = root.openNextFile();
      name = entry.name();
      // Serial.print("entry name: ");
      // Serial.print(name);
      // Serial.print(" ; curfile name: ");
      // Serial.print(*curfile);
      // Serial.print(" ; condition: ");
      // Serial.println(strcmp(name.c_str(), curfile->c_str()) != 0);
      entry.close();
    } while(strcmp(name.c_str(), curfile->c_str()) != 0);
    resultFile = root.openNextFile();
    if(resultFile) {
      resultPointer = new File(resultFile);
    };
    root.close();
    return resultPointer;
  };

  String getCurfile() { // Получить контекст текущего файла
    return *curfile;
  };

  /* 2 последние функции это костыль для вывода буфера на дисплей.
     Я никак не мог пометить текущий файл, поэтому пришлось сделать
     вот так. Обязательно нужно исправить
   */
  void bufBeforePrint() {
    char _curfilePosInBuf = curfilePosInBuf();
    // Serial.println(_curfilePosInBuf, DEC);
    String* _curfile = nullptr;
    if(_curfilePosInBuf != -1) {
      _curfile = buf[_curfilePosInBuf];
      *_curfile += "    ###";
    };
  };

  void bufAfterPrint(char pos) {
    String* _curfile = nullptr;
    if(pos != -1) {
      _curfile = buf[pos];
      _curfile->remove(_curfile->length() - 7, _curfile->length());
    };
  };
};
