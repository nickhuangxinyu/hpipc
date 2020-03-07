#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <assert.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include "shmdata.h"

using namespace std;
class CommonShm {
 public:
  CommonShm();
  ~CommonShm();

  void ConnectSpace();
 private:
  int size;
  int write_index;
  int read_index;
  int id;
  char* shmp;
};
