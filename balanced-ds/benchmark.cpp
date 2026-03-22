// Înainte de a compila acest program, decomentează una dintre structurile de
// date de mai jos.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int N = 500'000;
const int NUM_PASSES = 100;

// Fie un wrapper peste un set PBDS pentru benchmarking...
// #include "pbds/pbds.h"

// ... fie una dintre cele două implementări de treap...
// #include "treap/split-merge.h"
// #include "treap/split-merge-dyn-alloc.h"
// #include "treap/rotations.h"

// ... fie un RNG și o reprezentare pentru skip lists.
#include "skip-list/rand-mask.h"
// #include "skip-list/rand-naive.h"
// #include "skip-list/rand-05.h"
// #include "skip-list/rand-0375.h"
// #include "skip-list/rand-025.h"

#include "skip-list/buf.h"
// #include "skip-list/array.h"
// #include "skip-list/vector.h"

data_structure ds;
int perm[N];

void gen_perm() {
  // Amestecă mulțimea {0, 1, ..., N - 1}.
  for (int i = 0; i < N; i++) {
    int j = rand() % (i + 1);
    perm[i] = perm[j];
    perm[j] = i;
  }
}

void verify_all_ops() {
  ds.init();

  for (int i = 0; i < N; i++) {
    ds.insert(perm[i]);
    int j = rand() % N;
    assert(ds.contains(perm[j]) == (j <= i));
  }

  for (int i = 0; i < N; i++) {
    assert(ds.order_of(i) == i);
    assert(ds.kth_element(i) == i);
  }

  for (int i = 0; i < N; i++) {
    ds.erase(perm[i]);
    int j = rand() % N;
    assert(ds.contains(perm[j]) == (j > i));
  }
}

int main() {
  srand(time(NULL));

  printf("Benchmark pentru implementarea: %s\n", ds.get_description().c_str());
  printf("n = %d (%d operații / tură, %lld operații total)\n",
         N, 6 * N, 6LL * N * NUM_PASSES);

  for (int pass = 0; pass < NUM_PASSES; pass++) {
    gen_perm();
    printf("Tura %d/%d\n", pass + 1, NUM_PASSES);
    verify_all_ops();
  }

  return 0;
}
