//读共享内存
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

typedef struct _Teacher {
  char name[64];
  int age;
} Teacher;

int main() {
  int shmid = -1;
  key_t key = 0x2234;
  Teacher *p = NULL;
  sem_t *semr = NULL, *semw = NULL;
  semr = sem_open("sem_r", O_CREAT | O_RDWR, 0666, 0);
  if (semr == SEM_FAILED) {
      printf("errno = %d\n", errno );
      return -1;
  }

  semw = sem_open("sem_w", O_CREAT | O_RDWR, 0666, 1);
  if (semw == SEM_FAILED){
    printf("errno = %d\n", errno );
    return -1;
  }
  
  shmid = shmget(key, 0, 0 );
  if ( shmid == -1 ) {
    printf("shmget failed\n");
    perror("shmget err");
    return -1;
  }

  p = (Teacher*)shmat(shmid, NULL, 0);
  if (p == (Teacher*)(-1)) {
    printf("shmat failed\n");
    perror("shmat");
    return -1;
  }

  while(1) {
    sem_wait(semr);
    printf("name:%s\n", p->name);
    printf("age:%d\n", p->age);
    sem_post(semw);
  }

  //shmdt(p);
  return 0;
}
