#include <struct/market_snapshot.h>
#include <util/data_handler.h>
#include <util/time_controller.h>
#include <util/recver.h>
#include "shm_reader.h"
#include "control.h"

int main() {
  cout << sizeof(MarketSnapshot) << endl;
  printf("mode=%s\n", MODE==1 ? "SHM" : "ZMQ");
  int count = 0;
  ShmReader sr(1234, 10000000, "mode");
  TimeController tc;
  Recver r("sender");
  MarketSnapshot shot;
  while (count < 1043684) {
    // printf("count = %d\n", count);
    if (count++ == 2) {
      tc.StartTimer();
    }
    if (MODE == 1) {
      shot = sr.read();
    } else if (MODE == 2) {
      r.Recv(shot);
    } else {
      printf("unknown mode\n");
      exit(1);
    }
    if (count < NUM_SAMPLE) {
      timeval t;
      gettimeofday(&t, NULL);
      printf("%ld,%ld\n", t.tv_sec, t.tv_usec);
    }
  }
  tc.EndTimer("100w shot read");
}
