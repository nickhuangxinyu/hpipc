#include "shm_worker.h"

class ShmWriter: public ShmWorker {
 public:
  ShmWriter(int key, int size, std::string mode) {
    init(key, size);
  }

  ~ShmWriter() {

  }

  void write(const MarketSnapshot& shot) {
    int* tail = (int*)(m_data + 2*sizeof(int));
    memcpy(m_data+3*sizeof(int)+*tail*sizeof(MarketSnapshot), &shot, sizeof(MarketSnapshot));
    (*tail)++;
  }
};
