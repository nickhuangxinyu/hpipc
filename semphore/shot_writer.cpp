#include <struct/market_snapshot.h>
#include <util/data_handler.h>
#include <util/time_controller.h>
#include <util/common_tools.h>
#include <util/sender.hpp>
#include <chrono>
#include <fstream>
#include <thread>
#include <memory>
#include "shm_sender.hpp"
#include "control.h"

template <typename T>
class Shoter : public DataHandler {
 public:
  Shoter()
    : count(0),
      sw(new ShmSender<T> ("1234", SHM_SIZE)),
      sender(new Sender<MarketSnapshot>("sender")),
      f(MODE==1 ? new std::ofstream("wshm.csv", ios::out) : new std::ofstream("wzmq.csv", ios::out)) {
  }

  ~Shoter() {
    tc.EndTimer("shot cost");
    f.get()->close();
    delete sw;
  }

  void HandleShot(T* shot) override {
    // shot->Show(stdout);
    if (count < NUM_SAMPLE) {
      timeval t;
      gettimeofday(&t, NULL);
      char buffer[1024];
      snprintf(buffer, sizeof(buffer), "%ld,%ld\n", t.tv_sec, t.tv_usec);
      *f.get() << buffer;
    }
    if (count++ == 1) {
      tc.StartTimer();
    }
    if (MODE == 1) {
      sw->Send(*shot);
    } else if (MODE == 2) {
      sender->Send(*shot);
    } else {
      printf("unknown mode\n");
      exit(1);
    }
    busy_sleep(chrono::microseconds(5));
  }
 private:
  int count;
  ShmSender<T>* sw;
  TimeController tc;
  Sender<MarketSnapshot> * sender;
  std::unique_ptr<ofstream> f;
};

int main() {
  Shoter <MarketSnapshot> s;
  std::string file_path = "/running/2020-02-26/future2020-02-26.dat";
  s.LoadData(file_path);
}
