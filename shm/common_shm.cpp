#include "common_shm.h"

CommonShm::CommonShm() {

}

CommonShm::~CommonShm() {

}

void CommonShm::ConnectSpace() {
  // check if shm existed
  int shmid = shmget((key_t)id, sizeof(int)*3+size*sizeof(struct shared_use_st), 0666|IPC_CREAT);
  if(shmid == -1) {
  }
  printf("shmid is %d\n", shmid);
}
