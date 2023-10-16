// Prepare an array of <position, value>. At each pass, shift the positions by
// one. Write down the previous values. At the end of the algorithm, compare
// the naive approach with the compact approach.
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

const int ARRAY_SIZE = 1'000'000;
const int NUM_CHANGES = 100'000;
const int NUM_PASSES = 10'000;

typedef struct {
  int pos;
  bool val, prevVal;
} change;

change c[NUM_CHANGES];
bool v[ARRAY_SIZE];
unsigned char compact[ARRAY_SIZE / 8];
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

void compactSet(int pos, bool val) {
  unsigned char& c = compact[pos >> 3];
  unsigned char mask = 1 << (pos & 7);
  if (val) {
    c |= mask;
  } else {
    c &= ~mask;
  }
}

void compactSetNoBranching(int pos, bool val) {
  unsigned char& c = compact[pos >> 3];
  unsigned char mask = 1 << (pos & 7);

  // Almost intelligible: the first half clears the bit, the second half sets it.
  c = (c & ~mask) | (-val & mask);

  // Or, a bit more cryptically.
  // c ^= (-val ^ c) & mask;
}

bool compactGet(int pos) {
  return (compact[pos >> 3] >> (pos & 7)) & 1;
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

void runCompactNoBranching() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prevVal = compactGet(pos);
      compactSetNoBranching(pos, c[i].val);
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
  runCompact();
  reportTime("bitset");

  markTime();
  runCompactNoBranching();
  reportTime("bitset, no branching");

  markTime();
  runNaive();
  reportTime("naive");

  verify();

  return 0;
}
