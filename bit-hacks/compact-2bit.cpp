#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

const int ARRAY_SIZE = 1'000'000;
const int NUM_CHANGES = 100'000;
const int NUM_PASSES = 10'000;

typedef unsigned char u8;

typedef struct {
  int pos;
  u8 val, prevVal;
} change;

change c[NUM_CHANGES];
u8 v[ARRAY_SIZE];
u8 compact[ARRAY_SIZE / 4];
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
  int ops = NUM_CHANGES * NUM_PASSES * 2; // we measured reads + writes

  printf("%s: %lld ms for %d ops (%d Mops/sec)\n", msg, t - t0,
         ops, ops / millis / 1000);
}

void genLocations() {
  for (int i = 0; i < NUM_CHANGES; i++) {
    c[i].pos = rand() % (ARRAY_SIZE - NUM_PASSES);
    c[i].val = rand() % 2;
  }
}

void compactSet(int pos, u8 val) {
  u8& c = compact[pos >> 2];
  int shift = 2 * (pos & 3);
  u8 mask = 3 << shift;

  c = (c & ~mask) | (val << shift);
}

u8 compactGet(int pos) {
  u8& c = compact[pos >> 2];
  int shift = 2 * (pos & 3);
  return (c >> shift) & 3;
}

void runCompact() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prevVal = compactGet(pos);
      compactSet(pos, c[i].val);
    }
  }
}

void runNaive() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prevVal = v[pos];
      v[pos] = c[i].val;
    }
  }
}

void verify() {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (v[i] != compactGet(i)) {
      printf("Error: position %d, naive is %d, compact is %d.\n",
             i, v[i], compactGet(i));
      exit(1);
    }
  }
}

int main(void) {
  srand(time(NULL));
  genLocations();

  markTime();
  runNaive();
  reportTime("naive");

  markTime();
  runCompact();
  reportTime("2-bit");
  verify();

  return 0;
}
