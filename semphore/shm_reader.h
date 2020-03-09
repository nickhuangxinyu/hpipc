#include "shm_worker.h"

class ShmReader : public ShmWorker {
 public:
  ShmReader(int key, int size, std::string mode) {
    init(key, size);
    auto tail = (atomic_int*)(m_data + 2*sizeof(atomic_int));
    read_index = tail->load();
    printf("read_index is %d\n", read_index.load());
  }

  ~ShmReader() {

  }

  MarketSnapshot read() {
    MarketSnapshot* s = NULL;
    while (read_index.load() == ((atomic_int*)(m_data + 2*sizeof(int)))->load()) {
    }
    s = reinterpret_cast<MarketSnapshot*>(m_data+3*sizeof(atomic_int)+(read_index.load()%m_size)*sizeof(MarketSnapshot));
    read_index.fetch_add(1);
    return *s;
  }



 private:
  atomic<int> read_index;
  std::unique_ptr<ofstream> f;
};
