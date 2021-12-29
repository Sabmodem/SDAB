#include "reader.cc"

reader* rd = nullptr;

void buttonsHandler(char cmd, reader* rd) {
  switch (cmd) {
  case 'n': // Если нажата n
    rd->readPageForward(); // читаем вперед
    rd->print();
    break;
  case 'p': // если нажата p
    rd->readPageBackward(); // читаем назад
    rd->print();
    break;
  };
};

void setup() {
  rd = new reader("1.txt");
};

void loop() {
  if(Serial.available()) {
    buttonsHandler(Serial.read(), rd);
  };
};
