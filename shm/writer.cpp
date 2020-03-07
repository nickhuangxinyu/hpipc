#include "producer.h"

int main() {
  producer s(1234, 100);
  for (int i = 0; i < 10; i++) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", i);
    s.write(buffer);
  }
  while(1);
}
