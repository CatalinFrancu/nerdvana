// Remember that std::popcount requires the -std=c++20 compilation flag.
#include <bit>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

const int N = 100'000;
const int NUM_PASSES = 10'000;

const int mult32[32] = {
  0 ,  9,  1, 10, 13, 21,  2, 29,
  11, 14, 16, 18, 22, 25,  3, 30,
  8 , 12, 20, 28, 15, 17, 24,  7,
  19, 27, 23,  6, 26,  5,  4, 31,
};

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
  int millis = t - t0;
  int ops = N * NUM_PASSES;

  printf("%s: %lld ms pentru %d operații (%d Mops/sec)\n", msg, t - t0,
         ops, ops / millis / 1000);
}

void lookupInit() {
  for (int i = 2; i <= 0xff; i++) {
    lookup[i] = lookup[i / 2] + 1;
  }

  for (int i = 2; i <= 0xffff ; i++) {
    lookupBig[i] = lookupBig[i / 2] + 1;
  }
}

void logNaive() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      unsigned x = v[i];
      witness[i] = -1;
      while (x) {
        witness[i]++;
        x >>= 1;
      }
    }
  }
}

void logLookupTable1() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
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
}

void logLookupTable2() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
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
}

void logLookupTable3() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      unsigned x = v[i];
      c[i] = (x >= (1 << 16))
        ? (16 + lookupBig[x >> 16])
        : lookupBig[x];
    }
  }
}

void logBuiltin() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      c[i] = 31 - __builtin_clz(v[i]);
    }
  }
}

void logStl() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      c[i] = 31 - std::countl_zero(v[i]);
    }
  }
}

void logParallel() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
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
}

void logMult() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      unsigned x = v[i];
      x |= x >> 1;
      x |= x >> 2;
      x |= x >> 4;
      x |= x >> 8;
      x |= x >> 16;
      c[i] = mult32[x * 0x07C4ACDD >> 27];
    }
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
  logNaive(); // o dată pentru cache
  reportTime("inițializare");

  markTime();
  logNaive();
  reportTime("naiv");

  markTime();
  logLookupTable1();
  reportTime("tabel 1 (if)");
  verify();

  markTime();
  logLookupTable2();
  reportTime("tabel 2 (while)");
  verify();

  markTime();
  logLookupTable3();
  reportTime("tabel 3 (64 kb)");
  verify();

  markTime();
  logBuiltin();
  reportTime("__builtin_clz");
  verify();

  markTime();
  logStl();
  reportTime("std::countl_zero");
  verify();

  markTime();
  logParallel();
  reportTime("în paralel");
  verify();

  markTime();
  logMult();
  reportTime("cu înmulțire");
  verify();

  return 0;
}
