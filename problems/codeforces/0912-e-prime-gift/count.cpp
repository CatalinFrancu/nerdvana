#include <set>
#include <stdio.h>

typedef unsigned long long u64;

const u64 LIMIT = 1'000'000'000'000'000'000ull;
const int STEPS = 10'000'000;
// const int NUM_PRIMES = 16;
// const int PRIMES[] = {
//   2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53,
// };
const int NUM_PRIMES = 10;
const int PRIMES[] = {
  17, 19, 23, 29, 31, 37, 41, 43, 47, 53
};

int main() {
  std::set<u64> s;
  u64 m = 0;

  s.insert(1);

  int i = 0;
  while ((i < STEPS) && !s.empty()) {
    std::set<u64>::iterator it = s.begin();
    m = *it;
    s.erase(it);

    for (int j = 0; j < NUM_PRIMES; j++) {
      if ((__int128)m * PRIMES[j] <= LIMIT) {
        s.insert(m * PRIMES[j]);
      }
    }

    // Keep the size to a minimum so we don't run out of RAM
    while ((int)s.size() > STEPS - i)  {
      std::set<u64>::iterator it = s.end();
      it--;
      s.erase(it);
    }
    i++;
  }

  printf("%d: %llu ratio %llu\n", i, m, m / i);
  return 0;
}

// Useful results:
// Numbers up to 10^18 from 2...13 (6 primes): 958k.
// Numbers up to 10^18 from 17...53 (10 primes): 505k.
