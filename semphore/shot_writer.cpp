#include <struct/market_snapshot.h>
#include <util/data_handler.hpp>
#include <util/time_controller.h>
#include <util/common_tools.h>
#include <util/zmq_sender.hpp>
#include <chrono>
#include <fstream>
#include <thread>
#include <memory>
#include "shm_sender.hpp"
#include "control.h"

void busy_sleep(std::chrono::nanoseconds t) {
  auto end=high_resolution_clock::now() + t; // - overhead;
  while(true) if(high_resolution_clock::now() > end) break;
}

class Shoter : public DataHandler<MarketSnapshot> {
 public:
  Shoter()
    : count(0),
      sw(new ShmSender<MarketSnapshot> ("1234", SHM_SIZE)),
      sender(new ZmqSender<MarketSnapshot>("sender")),
      f(MODE==1 ? new std::ofstream("wshm.csv", ios::out) : new std::ofstream("wzmq.csv", ios::out)) {
  }

  ~Shoter() {
    tc.EndTimer("shot cost");
    f.get()->close();
    delete sw;
  }

  void HandleShot(MarketSnapshot* this_shot, MarketSnapshot* next_shot) override {
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
      sw->Send(*this_shot);
    } else if (MODE == 2) {
      sender->Send(*this_shot);
    } else {
      printf("unknown mode\n");
      exit(1);
    }
    busy_sleep(chrono::microseconds(5));
  }
 private:
  int count;
  ShmSender<MarketSnapshot>* sw;
  TimeController tc;
  ZmqSender<MarketSnapshot> * sender;
  std::unique_ptr<ofstream> f;
};

int main() {
  Shoter s;
  std::string file_path = "/home/xhuang/future2021-01-04.dat.gz";
  s.LoadData(file_path);
}
