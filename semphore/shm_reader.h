#include "shm_worker.h"

class ShmReader : public ShmWorker {
 public:
  ShmReader(int key, int size, std::string mode) {
    init(key, size);
    int* tail = (int*)(m_data + 2*sizeof(int));
    read_index = *tail;
    printf("read_index is %d\n", read_index);
  }

  ~ShmReader() {

  }

  MarketSnapshot read() {
    MarketSnapshot* s = NULL;
    while (read_index >= *(int*)(m_data + 2*sizeof(int))) {
    }
    s = reinterpret_cast<MarketSnapshot*>(m_data+3*sizeof(int)+read_index*sizeof(MarketSnapshot));
    read_index ++;
    return *s;
  }



 private:
  int read_index;
};
