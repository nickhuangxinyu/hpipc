#include <struct/market_snapshot.h>
#include <util/data_handler.h>
#include <util/time_controller.h>
#include <util/common_tools.h>
#include <util/sender.h>
#include <chrono>
#include <fstream>
#include <thread>
#include <memory>
#include "shm_writer.h"
#include "control.h"

class Shoter : public DataHandler {
 public:
  Shoter()
    : count(0),
      sw(new ShmWriter <MarketSnapshot> (1234, 10000, "mode")),
      sender(new Sender("sender")),
      f(MODE==1 ? new std::ofstream("wshm.csv", ios::out) : new std::ofstream("wzmq.csv", ios::out)) {
  }

  ~Shoter() {
    tc.EndTimer("shot cost");
    f.get()->close();
  }

  void HandleShot(MarketSnapshot* shot) override {
    // shot->Show(stdout);
    if (count++ == 1) {
      tc.StartTimer();
    }
    if (MODE == 1) {
      sw->write(*shot);
    } else if (MODE == 2) {
      sender->Send(*shot);
    } else {
      printf("unknown mode\n");
      exit(1);
    }
    if (count < NUM_SAMPLE) {
      timeval t;
      gettimeofday(&t, NULL);
      char buffer[1024];
      snprintf(buffer, sizeof(buffer), "%ld,%ld\n", t.tv_sec, t.tv_usec);
      *f.get() << buffer;
    }
    busy_sleep(chrono::microseconds(1));
  }
 private:
  int count;
  ShmWriter<MarketSnapshot>* sw;
  TimeController tc;
  Sender * sender;
  std::unique_ptr<ofstream> f;
};

int main() {
  Shoter s;
  std::string file_path = "/running/2020-02-26/future2020-02-26.dat";
  s.LoadData(file_path);
}
