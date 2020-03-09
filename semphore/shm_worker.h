#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <struct/market_snapshot.h>
#include <atomic>
#include <memory>

using namespace std;

class ShmWorker {
 public:
  ShmWorker() : is_init(false) {

  }

  ~ShmWorker() {

  }


 protected:
  template <typename T>
  void init(int key, int size) {
    if (is_init) {
      printf("this shmworker has beed inited!\n");
      return;
    }
    m_key = key;
    int shmid = shmget(m_key, 0, 0);
    if (shmid == -1) {
      perror("shmget err");
      if (errno == ENOENT) {
        shmid = shmget(m_key, 3*sizeof(std::atomic_int)+sizeof(T)*size, 0666 | IPC_CREAT | O_EXCL);
        printf("creating new shm\n");
        m_data = (char*)shmat(shmid, NULL, 0);
        m_size = size;
        *reinterpret_cast<atomic_int*>(m_data) = m_size;
        *reinterpret_cast<atomic_int*>(m_data + sizeof(std::atomic_int)) = -1;
        *reinterpret_cast<atomic_int*>(m_data + 2*sizeof(std::atomic_int)) = 0;
      } else {
        exit(1);
      }
    } else {
      m_data = (char*)shmat(shmid, 0, 0);
      m_size = reinterpret_cast<atomic_int*>(m_data)->load();
      printf("connecting to an exsited shm!\n");
    }
  
    if (m_data == (char*)(-1)) {
      printf("shmat failed\n");
      perror("shmat");
      exit(1);
    }
    is_init = true;
  }

  int m_key;
  int m_size;
  char* m_data;
  bool is_init;
};
