#include <struct/market_snapshot.h>
#include <util/data_handler.h>
#include <util/time_controller.h>
#include <util/recver.h>
#include <memory>
#include "shm_reader.h"
#include "control.h"

int main() {
  printf("mode=%s\n", MODE==1 ? "SHM" : "ZMQ");
  unique_ptr<ofstream> f(MODE==1 ? new std::ofstream("rshm.csv", ios::out) : new std::ofstream("rzmq.csv", ios::out));
  int count = 0;
  ShmReader<MarketSnapshot> sr(1234, 10000);
  TimeController tc;
  Recver r("sender");
  MarketSnapshot shot;
  while (count < 1043684) {
    if (count++ == 2) {
      tc.StartTimer();
    }
    if (MODE == 1) {
      shot = sr.read();
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
      char buffer[1024];
      snprintf(buffer, sizeof(buffer), "%ld,%ld\n", t.tv_sec, t.tv_usec);
      *f.get() << buffer;
    }
  }
  tc.EndTimer("100w shot read");
}
