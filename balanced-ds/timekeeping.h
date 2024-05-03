#include <sys/time.h>

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

long long t0;

void mark_time() {
  timeval tv;
  gettimeofday(&tv, NULL);
  t0 = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
}

void report_time(const char* msg) {
  timeval tv;

  gettimeofday(&tv, NULL);
  long long t = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
  int ops = N * NUM_PASSES;

  printf("%s: %lld ms pentru %d operații\n", msg, t - t0, ops);
}
