#include "shm_writer.h"

#include <iostream>
using namespace std;
int main() {
  ShmWriter sw(1234, 100, "a");
  std::string a;
  while (cin >> a && a != "exit") {
    MarketSnapshot shot;
    snprintf(shot.ticker, sizeof(shot.ticker), "%s", a.c_str());
    sw.write(shot);
    printf("write %s\n", shot.ticker);
  }
}
