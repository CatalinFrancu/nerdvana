#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>

#include "skip-list/rand-naive.h"
// #include "skip-list/rand-05.h"
// #include "skip-list/rand-0375.h"
// #include "skip-list/rand-025.h"

// #include "basic-array.h"
#include "skip-list/basic-buf.h"
// #include "skip-list/basic-vector.h"

const int N = 300'000;
const int NUM_PASSES = 10;
const int MAX_VALUE = 1'000'000'000;

#include "timekeeping.h"

skip_list sl;
std::set<int> stl_set;
int witness[N];

void gen_witnesses() {
  // Shuffle the set (0, 1, ..., N-1).
  for (int i = 0; i < N; i++) {
    int j = rand() % (i + 1);
    witness[i] = witness[j];
    witness[j] = i;
  }
}

void benchmark_insert_naive() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    stl_set.clear();
    for (int i = 0; i < N; i++) {
      stl_set.insert(witness[i]);
    }
  }
}

void benchmark_insert_skip_list() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    sl.init();
    for (int i = 0; i < N; i++) {
      sl.insert(witness[i]);
    }
  }
}

void benchmark_find_naive() {
  // Insert half the elements in the skip list.
  stl_set.clear();
  for (int i = 0; i < N; i += 2) {
    stl_set.insert(witness[i]);
  }

  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      bool exists = stl_set.find(witness[i]) != stl_set.end();
      assert(exists != i % 2);
    }
  }
}

void benchmark_find_skip_list() {
  // Insert half the elements in the skip list.
  sl.init();
  for (int i = 0; i < N; i += 2) {
    sl.insert(witness[i]);
  }

  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      bool exists = sl.contains(witness[i]);
      assert(exists != i % 2);
    }
  }
}

int main() {

  init_rng();

  gen_witnesses();

  printf("Metoda: %s\n", METHOD);
  printf("Metoda RNG: %s\n", RAND_METHOD);

  mark_time();
  benchmark_insert_naive();
  report_time("inserare naiv");

  mark_time();
  benchmark_insert_skip_list();
  report_time("inserare skip list");

  mark_time();
  benchmark_find_naive();
  report_time("căutare naiv");

  mark_time();
  benchmark_find_skip_list();
  report_time("căutare skip list");

  return 0;
}
