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

class consumer {
 public:
  consumer(int id, int size=100) : id(id), size(size), read_index(0), write_index(0) {
    allocate();
    connect();
  }

  ~consumer() {
    if(shmdt(shmp) == -1) {
      fprintf(stderr, "shmdt failed\n");
      exit(EXIT_FAILURE);
    }   
    //删除共享内存
    if(shmctl(shmid, IPC_RMID, 0) == -1) {
      fprintf(stderr, "shmctl(IPC_RMID) failed\n");
      exit(EXIT_FAILURE);
    }
  }

  void allocate() {
    shmid = shmget((key_t)id, sizeof(int)*3+size*sizeof(struct shared_use_st), 0666|IPC_CREAT);
    if(shmid == -1) {
      fprintf(stderr, "shmget failed\n");
      exit(EXIT_FAILURE);
    }
    printf("shmid is %d\n", shmid);
  }

  std::string read() {
    updateindex();
    if (read_index >= write_index) {
      return "empty";
    }
    struct shared_use_st* a = reinterpret_cast<shared_use_st*>(shmp + 3*sizeof(int) + sizeof(shared_use_st)*read_index);
    read_index = (read_index+1) % size;
    memcpy(shmp+2*sizeof(int), &read_index, sizeof(int));
    return a->text;
  }

  void connect() {
    shmp = (char*)shmat(shmid, (void*)0, 0);
    if(shmp == (void*)-1) {
      fprintf(stderr, "shmat failed\n");
      allocate();
      shmp = (char*)shmat(shmid, (void*)0, 0);
      // exit(EXIT_FAILURE);
    }
    memcpy(shmp, &size, sizeof(int));
    memcpy(shmp+sizeof(int), &read_index, sizeof(int));
    memcpy(shmp+2*sizeof(int), &write_index, sizeof(int));
    updateindex();
  }

  void updateindex() {
    size = *reinterpret_cast<int*>(shmp);
    read_index = *reinterpret_cast<int*>(shmp+sizeof(int));
    write_index = *reinterpret_cast<int*>(shmp+2*sizeof(int));
  }

 private:
  int id;
  int size;
  int shmid;
  char* shmp;
  int read_index;
  int write_index;
};
