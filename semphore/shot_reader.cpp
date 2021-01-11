#include <struct/market_snapshot.h>
#include <util/time_controller.h>
#include <util/zmq_recver.hpp>
#include <memory>
#include "shm_recver.hpp"
#include "control.h"

int main() {
  printf("mode=%s\n", MODE==1 ? "SHM" : "ZMQ");
  unique_ptr<ofstream> f(MODE==1 ? new std::ofstream("rshm.csv", ios::out) : new std::ofstream("rzmq.csv", ios::out));
  int count = 0;
  ShmRecver<MarketSnapshot> sr("1234", SHM_SIZE);
  TimeController tc;
  ZmqRecver<MarketSnapshot> r("sender");
  MarketSnapshot shot;
  while (count < 1043684) {
  // while (true) {
    // printf("count is %d\n", count);
    if (count++ == 2) {
      tc.StartTimer();
    }
    if (MODE == 1) {
      sr.Recv(shot);
      // shot.Show(stdout);
    } else if (MODE == 2) {
      r.Recv(shot);
    } else {
      printf("unknown mode\n");
      exit(1);
    }
    if (count < NUM_SAMPLE) {
      timeval t;
      gettimeofday(&t, NULL);
      // char buffer[1024];
      // snprintf(buffer, sizeof(buffer), "%ld,%ld\n", t.tv_sec, t.tv_usec);
      *f.get() << t.tv_sec;
      *f.get() << ",";
      *f.get() << t.tv_usec << endl;
    }
  }
  tc.EndTimer("100w shot read");
}
