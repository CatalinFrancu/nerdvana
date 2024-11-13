// For this problem, bit masks store one bit of information per letter of the
// alphabet. First, compute all obtainable bitmasks: all sequences of length
// up to 20, consisting of distinct letters, that occur somewhere in the
// string.
//
// Next, for each (not necessarily obtainable) mask m, compute the largest
// population (most bits set) submask. That is, the longest substring of s
// that consists only of letters indicated by m.
//
// Finally, iterate through all 2^20 masks m and find the maximum value of
// pop(m) + submask_pop(~m). That is, the number of bits set in m and the most
// bits we can set from among the complement of m.
#include <stdio.h>
#include <ctype.h>

const int MAX_LEN = 1'000'000;
const int SIGMA = 20;

char s[MAX_LEN + 1];
bool obtainable[1 << SIGMA];
char submask_pop[1 << SIGMA];
int len;

void read_data() {
  char c;
  len = 0;
  while (isalpha((c = getchar()))) {
    s[len++] = c - 'a';
  }
}

void build_obtainable_masks() {
  for (int i = 0; i < len; i++) {
    int mask = 0;
    int j = i;
    while ((j < len) && !(mask & (1 << s[j]))) {
      mask |= 1 << s[j++];
      obtainable[mask] = true;
    }
  }
}

void improve(char& x, int y) {
  if (y > x) {
    x = y;
  }
}

void build_submask_populations() {
  for (int mask = 1; mask < (1 << SIGMA); mask++) {
    if (obtainable[mask]) {
      submask_pop[mask] = __builtin_popcount(mask);
    } else {
      for (int bit = 0; bit < SIGMA; bit++) {
        if (mask & (1 << bit)) {
          int s = submask_pop[mask ^ (1 << bit)];
          improve(submask_pop[mask], s);
        }
      }
    }
  }
}

void find_pair() {
  char ans = 0;

  for (int mask = 1; mask < (1 << SIGMA); mask++) {
    if (obtainable[mask]) {
      int complement = (1 << SIGMA) - 1 - mask;
      improve(ans, __builtin_popcount(mask) + submask_pop[complement]);
    }
  }

  printf("%d\n", ans);
}

int main() {
  read_data();
  build_obtainable_masks();
  build_submask_populations();
  find_pair();

  return 0;
}
