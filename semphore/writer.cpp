//写共享内存
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h> //declare O_CREAT O_RDWR

#include <struct/market_snapshot.h>

int main() {
  std::string shm_name = "sem_read";
  // sem_unlink(shm_name.c_str());
  auto semr = sem_open(shm_name.c_str(), O_CREAT, 0666, 0);
  if (semr == SEM_FAILED) {
    printf("errno = %d\n", errno );
    return -1;
  }
  key_t key = 0x2234;
  int max = 10000;
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
      *reinterpret_cast<int*>(p) = max;
      *reinterpret_cast<int*>(p+sizeof(int)) = -1;
      *reinterpret_cast<int*>(p+2*sizeof(int)) = 0;
    } else {
      exit(1);
    }
  } else {
    p = (char*)shmat(shmid, NULL, 0);
    printf("connecting to an exsited shm!\n");
  }

  if (p == (char*)(-1)) {
    printf("shmat failed\n");
    perror("shmat");
    return -1;
  }
  int * write_index = (int*)(p+2*sizeof(int));
  while (*write_index < max) {
    auto c = reinterpret_cast<MarketSnapshot*>(p+(*write_index)++*sizeof(MarketSnapshot));
    printf("write index = %d\n", *write_index);
    c->bid_sizes[0] = *write_index;
    c->Show(stdout);
    sem_post(semr);
    sleep(1);
  }
  while (1);
}
