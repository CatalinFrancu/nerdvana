// Method: try to use every possible number of credits at every position.
#include <stdio.h>
#include <stdlib.h>

#define MAX_N 1'000'000

// Whenever we run out of credits midway through the array, suf[] stores the
// precomputed ugliness of the remaining elements.
long long suf[MAX_N], k, best;
int v[MAX_N], n;

long long min(long long x, long long y) {
  return (x < y) ? x : y;
}

void bkt(int level, long long ugliness, long long credit) {
  if (level == n) {
    // base case: we made it all the way to the end
    best = min(best, ugliness);
    return;
  }

  if (ugliness > best) {
    // base case: we exceeded the best cost so far
    return;
  }

  if (!credit) {
    // base case: out of credits
    if (level) {
      ugliness += abs(v[level] - v[level - 1]); // left
    }
    if (level < n - 1) {
      ugliness += abs(v[level] - v[level + 1]); // right
    }
    ugliness += suf[level + 1]; // rest of array
    best = min(best, ugliness);
    return;
  }

  for (int i = 0; i <= credit; i++) {
    v[level] += i;
    bkt(level + 1,
        ugliness + (level ? abs(v[level] - v[level - 1]) : 0),
        credit - i);
    v[level] -= i;
  }
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    scanf("%d %lld", &n, &k);
    for (int i = 0; i < n; i++) {
      scanf("%d", &v[i]);
    }

    // precompute partial uglinesses...es
    suf[n - 1] = 0;
    for (int i = n - 2; i >= 0; i--) {
      suf[i] = suf[i + 1] + abs(v[i] - v[i + 1]);
    }

    best = suf[0];
    bkt(0, 0, k);
    printf("%lld\n", best);
  }

  return 0;
}
