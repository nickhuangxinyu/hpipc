#include "shm_reader.h"

#include <iostream>
using namespace std;
int main() {
  ShmReader sr(1234, 100, "a");
  std::string a;
  while (true) {
    auto shot = sr.read();
    shot.Show(stdout);
  }
}
