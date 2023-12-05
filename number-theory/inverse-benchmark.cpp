#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MOD = 1'000'000'007;
const int N = 10'000;
const int NUM_PASSES = 10'000;

long long t0;
int v[N], inv[N];

void mark_time() {
  timeval tv;

  gettimeofday (&tv, NULL);
  t0 = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
}

void report_time(const char* msg) {
  timeval tv;

  gettimeofday (&tv, NULL);
  long long t = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
  int millis = t - t0;
  int ops = N * NUM_PASSES;

  printf("%s: %lld ms pentru %d inverse (%.1lf M inverse/sec)\n", msg, t - t0,
         ops, (double)ops / millis / 1000);
}

void init_rng() {
  timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

void gen_array() {
  for (int i = 0; i < N; i++) {
    v[i] = rand() % MOD;
  }
}

int mod_pow(int b, int e) {
  long long result = 1;

  while (e) {
    if (e & 1) {
      result = result * b % MOD;
    }
    b = (long long)b * b % MOD;
    e >>= 1;
  }

  return result;
}

void extended_euclid_recursive(int a, int b, int& d, int& x, int& y) {
  if (!b) {
    d = a;
    x = 1;
    y = 0;
  } else {
    int xp, yp;
    extended_euclid_recursive(b, a % b, d, xp, yp);
    x = yp;
    y = xp - (a / b) * yp;
  }
}

void extended_euclid_iterative(int a, int b, int& d, int& x, int& y) {
  x = 1;
  y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = x - q * xp; x = tmp;
    tmp = yp; yp = y - q * yp; y = tmp;
  }
  d = a;
}

void benchmark_fermat() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      inv[i] = mod_pow(v[i], MOD - 2);
    }
  }
}

void benchmark_euclid_recursive() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      int y, k, d;
      extended_euclid_recursive(v[i], MOD, d, y, k);
      inv[i] = (y >= 0) ? y : (y + MOD);
    }
  }
}

void benchmark_euclid_iterative() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      int y, k, d;
      extended_euclid_iterative(v[i], MOD, d, y, k);
      inv[i] = (y >= 0) ? y : (y + MOD);
    }
  }
}

void verify() {
  for (int i = 0; i < N; i++) {
    assert((long long)v[i] * inv[i] % MOD == 1);
  }
}

int main() {
  init_rng();
  gen_array();

  mark_time();
  benchmark_fermat();
  report_time("Fermat");
  verify();

  mark_time();
  benchmark_euclid_recursive();
  report_time("Euclid recursiv");
  verify();

  mark_time();
  benchmark_euclid_iterative();
  report_time("Euclid iterativ");
  verify();

  return 0;
}
