#include "spsc_queue.hpp"
#include <iostream>

using namespace std;

int main() {
  FIFO_Queue<int, 32> q;
  int a = 3;
  int b;
  q.push(a);
  cout << q.pop(b) << endl;
  cout << b<< endl;
}
