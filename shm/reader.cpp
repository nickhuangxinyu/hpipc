#include "consumer.h"

int main() {
  consumer s(1234);
  /*
  for (int i = 0; i < 10; i++) {
    printf("read %s out\n", s.read().c_str());
  }
  */
  while (s.read()=="empty") {
  }
  for (int i = 0; i < 9; i++) {
    printf("read %s out\n", s.read().c_str());
  }
}
