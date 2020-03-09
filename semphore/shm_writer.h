#include "shm_worker.h"

template <typename T>
class ShmWriter: public ShmWorker {
 public:
  ShmWriter(int key, int size, std::string mode) {
    init <T> (key, size);
  }

  ~ShmWriter() {

  }

  void write(const T& shot) {
    auto tail = (atomic_int*)(m_data + 2*sizeof(atomic_int));
    memcpy(m_data+3*sizeof(atomic_int)+(tail->load()%m_size)*sizeof(T), &shot, sizeof(T));
    tail->fetch_add(1);
  }
 private:
};
