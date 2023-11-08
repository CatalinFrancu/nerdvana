// Remember that std::popcount requires the -std=c++20 compilation flag.
#include <assert.h>
#include <bit>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

const int N = 10'000;
const int NUM_PASSES = 10'000;

unsigned v[N];
int witness[N], c[N];
int lookup[256];
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
  for (int i = 0; i < 256; i++) {
    lookup[i] = lookup[i / 2] + (i & 1);
  }
}

void pcNaive() {
  int pop;
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      unsigned x = v[i];
      pop = 0;
      while (x) {
        pop += x & 1;
        x >>= 1;
      }
      witness[i] = pop;
    }
  }
}

void pcKernighan() {
  int pop;
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      unsigned x = v[i];
      pop = 0;
      while (x) {
        pop++;
        x &= x - 1;
      }
      c[i] = pop;
    }
  }
}

void pcLookupTable1() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      c[i] = lookup[v[i] & 0xff] +
        lookup[(v[i] >> 8) & 0xff] +
        lookup[(v[i] >> 16) & 0xff] +
        lookup[(v[i] >> 24) & 0xff];
    }
  }
}

void pcLookupTable2() {
  unsigned char* p;
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      p = (unsigned char*) &v[i];
      c[i] = lookup[p[0]] + lookup[p[1]] + lookup[p[2]] + lookup[p[3]];
    }
  }
}

void pcBuiltin() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      c[i] = __builtin_popcount(v[i]);
    }
  }
}

void pcStl() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      c[i] = std::popcount(v[i]);
    }
  }
}

void pcParallel() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      unsigned x = v[i];
      //    x = x - ((x >> 1) & 0x55555555); // rescrisă pentru claritate
      x = ((x >> 1) & 0x55555555) + (x & 0x55555555);
      x = ((x >> 2) & 0x33333333) + (x & 0x33333333);
      x = ((x >> 4) + x) & 0x0f0f0f0f;
      x = ((x >> 8) + x) & 0x00ff00ff;
      x = ((x >> 16) + x) & 0x0000ffff;
      c[i] = x;
    }
  }
}

// verificare de corectitudine
void verify() {
  for (int i = 1; i < N; i++) {
    assert(c[i] == witness[i]);
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
  pcKernighan();
  reportTime("Kernighan");
  verify();

  markTime();
  pcLookupTable1();
  reportTime("tabel + shift");
  verify();

  markTime();
  pcLookupTable2();
  reportTime("tabel + char*");
  verify();

  markTime();
  pcBuiltin();
  reportTime("__builtin_popcount");
  verify();

  markTime();
  pcStl();
  reportTime("std::popcount");
  verify();

  markTime();
  pcParallel();
  reportTime("în paralel");
  verify();

  return 0;
}
