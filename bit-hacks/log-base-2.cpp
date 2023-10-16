// Remember that std::popcount requires the -std=c++20 compilation flag.
#include <bit>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

const int N = 100'000'000;

unsigned v[N];
int witness[N], c[N];
int lookup[1 << 8];
int lookupBig[1 << 16];
long long t0;

void markTime() {
  timeval tv;

  gettimeofday (&tv, NULL);
  t0 = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
}

void reportTime(const char* msg) {
  timeval tv;

  gettimeofday (&tv, NULL);
  long long t = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
  printf("%s: %lld ms\n", msg, t - t0);
}

void lookupInit() {
  for (int i = 2; i <= 0xff; i++) {
    lookup[i] = lookup[i / 2] + 1;
  }

  for (int i = 2; i <= 0xffff ; i++) {
    lookupBig[i] = lookupBig[i / 2] + 1;
  }
}

void pcNaive() {
  for (int i = 0; i < N; i++) {
    unsigned x = v[i];
    witness[i] = -1;
    while (x) {
      witness[i]++;
      x >>= 1;
    }
  }
}

void pcLookupTable1() {
  for (int i = 0; i < N; i++) {
    unsigned x = v[i];
    c[i] = (x >= (1 << 16))
      ? ((x >= (1 << 24))
         ? (24 + lookup[x >> 24])
         : (16 + lookup[x >> 16]))
      : ((x >= (1 << 8))
         ? (8 + lookup[x >> 8])
         : lookup[x]);
  }
}

void pcLookupTable2() {
  for (int i = 0; i < N; i++) {
    unsigned x = v[i];
    int log = 0;
    while (x >= 256) {
      x >>= 8;
      log += 8;
    }
    log += lookup[x];
    c[i] = log;
  }
}

void pcLookupTable3() {
  for (int i = 0; i < N; i++) {
    unsigned x = v[i];
    c[i] = (x >= (1 << 16))
      ? (16 + lookupBig[x >> 16])
      : lookupBig[x];
  }
}

void pcBuiltin() {
  for (int i = 0; i < N; i++) {
    c[i] = 31 - __builtin_clz(v[i]);
  }
}

void pcStl() {
  for (int i = 0; i < N; i++) {
    c[i] = 31 - std::countl_zero(v[i]);
  }
}

void pcParallel() {
  for (int i = 0; i < N; i++) {
    unsigned x = v[i];
    int log = 0;

    if (x > 0xffff) {
      log += 16;
      x >>= 16;
    }
    if (x > 0xff) {
      log += 8;
      x >>= 8;
    }
    if (x > 0xf) {
      log += 4;
      x >>= 4;
    }
    if (x > 0x3) {
      log += 2;
      x >>= 2;
    }
    if (x > 1) {
      log += 1;
      x >>= 1;
    }
    c[i] = log;
  }
}

// verificare de corectitudine
void verify() {
  for (int i = 1; i < N; i++) {
    if (c[i] != witness[i]) {
      printf("Error: value is %u, witness log is %d, method log is %d.\n",
             v[i], witness[i], c[i]);
      exit(1);
    }
  }
}

int main(void) {
  srand(time(NULL));
  for (int i = 0; i < N; i++) {
    v[i] = (((unsigned)(rand() & 0xffff)) << 16) + (rand() & 0xffff);
  }
  lookupInit();

  markTime();
  pcNaive(); // o dată pentru cache
  reportTime("inițializare");

  markTime();
  pcNaive();
  reportTime("naiv");

  markTime();
  pcLookupTable1();
  reportTime("tabel 1");
  verify();

  markTime();
  pcLookupTable2();
  reportTime("tabel 2");
  verify();

  markTime();
  pcLookupTable3();
  reportTime("tabel 3 (64 kb)");
  verify();

  markTime();
  pcBuiltin();
  reportTime("__builtin_clz");
  verify();

  markTime();
  pcStl();
  reportTime("std::countl_zero");
  verify();

  markTime();
  pcParallel();
  reportTime("în paralel");
  verify();

  return 0;
}
