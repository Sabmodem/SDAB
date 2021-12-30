#define bufsize      6
// static const char bufsize = 6;

class browser {
 private:
  String buf[bufsize]; // размер буфера вывода в строках
  String* curfile; // указатель на строку с названием текущего файла
  struct fileContext {
    /*
      Контекст файла. Содержит открытый коренной каталог и сам файл,
      а также методы для вывода и закрытия каталога и файла
    */
    File root;
    File curfile;
    void print() const;
    void close();
  };
 public:
  browser();
  ~browser();
  void print();
  void printO();
  File getFirstFile();
  void fillFirstBuf();
  void clearBuf();
  void moveBufUp();
  void moveBufDown();
  void moveCurfileUp();
  void moveCurfileDown();
  char curfilePosInBuf();
  File prevFile();
  File nextFile();
  fileContext curfileContext();
  String* getCurfile();
  String* getBuf();
};
