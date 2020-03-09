#include "shm_worker.h"

class ShmWriter: public ShmWorker {
 public:
  ShmWriter(int key, int size, std::string mode) {
    init(key, size);
  }

  ~ShmWriter() {

  }

  void write(const MarketSnapshot& shot) {
    auto tail = (atomic_int*)(m_data + 2*sizeof(atomic_int));
    memcpy(m_data+3*sizeof(atomic_int)+(tail->load()%m_size)*sizeof(MarketSnapshot), &shot, sizeof(MarketSnapshot));
    tail->fetch_add(1);
  }
 private:
};
