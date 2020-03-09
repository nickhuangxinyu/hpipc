#include <struct/market_snapshot.h>
#include <util/data_handler.h>
#include <util/time_controller.h>
#include <util/common_tools.h>
#include <util/sender.h>
#include <chrono>
#include <thread>
#include "shm_writer.h"
#include "control.h"

class Shoter : public DataHandler {
 public:
  Shoter()
    : count(0),
      sw(new ShmWriter(1234, 10000000, "mode")),
      sender(new Sender("sender")) {
  }

  ~Shoter() {
    tc.EndTimer("shot cost");
  }

  void HandleShot(MarketSnapshot* shot) override {
    // printf("count=%d\n", count);
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
      printf("%ld,%ld\n", t.tv_sec, t.tv_usec);
    }
    // std::this_thread::sleep_for(chrono::nanoseconds(1));
    busy_sleep(chrono::microseconds(1));
  }
 private:
  int count;
  ShmWriter* sw;
  TimeController tc;
  Sender * sender;
};

int main() {
  Shoter s;
  std::string file_path = "/running/2020-02-26/future2020-02-26.dat";
  s.LoadData(file_path);
}
