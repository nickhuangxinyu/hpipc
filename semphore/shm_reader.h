#include "shm_worker.h"

template <typename T>
class ShmReader : public ShmWorker {
 public:
  ShmReader(int key, int size) {
    init <T> (key, size);
    auto tail = (atomic_int*)(m_data + 2*sizeof(atomic_int));
    read_index = tail->load();
    printf("read_index is %d\n", read_index.load());
  }

  ~ShmReader() {

  }

  T read() {
    T* s = NULL;
    while (read_index.load() == ((atomic_int*)(m_data + 2*sizeof(int)))->load()) {
    }
    s = reinterpret_cast<T*>(m_data+3*sizeof(atomic_int)+(read_index.load()%m_size)*sizeof(T));
    read_index.fetch_add(1);
    return *s;
  }



 private:
  atomic<int> read_index;
  std::unique_ptr<ofstream> f;
};
