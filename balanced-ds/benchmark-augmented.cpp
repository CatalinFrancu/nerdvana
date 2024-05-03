#include <assert.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <stdio.h>
#include <stdlib.h>

#include "skip-list/rand-naive.h"
// #include "skip-list/rand-05.h"
// #include "skip-list/rand-0375.h"
// #include "skip-list/rand-025.h"

// #include "skip-list/augmented-array.h"
#include "skip-list/augmented-buf.h"
// #include "skip-list/augmented-vector.h"

typedef __gnu_pbds::tree<
  int,
  __gnu_pbds::null_type,
  std::less<int>,
  __gnu_pbds::rb_tree_tag,
  __gnu_pbds::tree_order_statistics_node_update
  > ordered_set;

const int N = 300'000;
const int NUM_PASSES = 10;
const int MAX_VALUE = 1'000'000'000;

#include "timekeeping.h"

skip_list sl;
ordered_set stl_set;
int perm[N];

void gen_perm() {
  // Shuffle the set (0, 1, ..., N-1).
  for (int i = 0; i < N; i++) {
    int j = rand() % (i + 1);
    perm[i] = perm[j];
    perm[j] = i;
  }
}

void benchmark_naive() {
  stl_set.clear();
  for (int i = 0; i < N; i++) {
    stl_set.insert(perm[i]);
  }

  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      int x = perm[i];
      assert((int)stl_set.order_of_key(x) == x);
      assert(*stl_set.find_by_order(x) == x);
    }
  }
}

void benchmark_skip_list() {
  sl.init();
  for (int i = 0; i < N; i++) {
    sl.insert(perm[i]);
  }

  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < N; i++) {
      int x = perm[i];
      assert(sl.order_of(x) == x);
      assert(sl.kth_element(x) == x);
    }
  }
}

int main() {

  init_rng();

  gen_perm();

  printf("Metoda: %s     niveluri: %d     RNG: %s\n",
         METHOD, MAX_LEVELS, RAND_METHOD);

  mark_time();
  benchmark_naive();
  report_time("order statistics naiv");

  mark_time();
  benchmark_skip_list();
  report_time("order statistics skip list");

  return 0;
}
