//读共享内存
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <struct/market_snapshot.h>

int main() {
  key_t key = 0x2234;
  int size =1000;
  std::string shm_name = "sem_read";
  sem_unlink(shm_name.c_str());
  sem_t*  semr = sem_open(shm_name.c_str(), O_CREAT|O_EXCL, 0666, 0);
  if (semr == SEM_FAILED) {
    printf("errno = %d\n", errno );
    if (errno == EEXIST) {
      semr = sem_open(shm_name.c_str(), O_CREAT, 0666, 0);
    }
  }

  char * p = NULL;
  int shmid = shmget(key, 0, 0);
  printf("shmid is %d, errno is %d\n", shmid, errno);
  if ( shmid == -1 ) {
    perror("shmget err");
    printf("errno is %d\n", errno);
    if (errno == ENOENT) {
      int shmid = shmget(key, 3*sizeof(int)+sizeof(MarketSnapshot)*10000000, 0666 | IPC_CREAT | O_EXCL);
      printf("creating new shm\n");
      p = (char*)shmat(shmid, NULL, 0);
      *reinterpret_cast<int*>(p) = size;
      *reinterpret_cast<int*>(p+sizeof(int)) = -1;
      *reinterpret_cast<int*>(p+2*sizeof(int)) = 0;
    } else {
      exit(1);
    }
  } else {
    printf("connecting to an exsited shm!\n");
  }

  if (p == (char*)(-1)) {
    printf("shmat failed\n");
    perror("shmat");
    return -1;
  }

  while(1) {
    sem_wait(semr);
    int* read_pos = (int*)(p+sizeof(int));
    printf("read_pos = %d\n", *read_pos);
    auto c = reinterpret_cast<MarketSnapshot*>(p+(++(*read_pos))*sizeof(MarketSnapshot));
    c->Show(stdout);
  }

  //shmdt(p);
  return 0;
}
