// Method: Meet in the middle.
//
// Nomenclature: the numbers in this task are called ugly numbers.
//
// Generate and sort all ugly numbers using the first 6/16*n primes in an
// array ugly1 and all ugly numbers using the remaining primes in an array
// ugly2. This ensures that both arrays have less than 1 million numbers
// (determined experimentally).
//
// Now any ugly number for the task will be the product of two numbers from
// ugly1 and ugly2 respectively. To find the kth ugly number, use binary
// search by x and ask the question: how many ugly numbers are there between 1
// and x? We can answer this question with the two pointer technique. An ugly
// number y from ugly1 can pair up with all the numbers z ≤ x/y from ugly2.
#include <algorithm>
#include <stdio.h>

typedef unsigned long long u64;

const int MAX_PRIMES = 16;
const int LEFT_FRAC = 6;
const int MAX_NUM_UGLY = 1'000'000;
const u64 LIMIT = 1e18;

int primes[MAX_PRIMES];
int num_primes, mid;
u64 kth;
u64 ugly1[MAX_NUM_UGLY], ugly2[MAX_NUM_UGLY];
int num_ugly1, num_ugly2;

struct ugly_generator {
  int ptr, last;
  u64* v;

  void rec(int k, u64 prod) {
    if (k == last) {
      v[ptr++] = prod;
    } else {
      for (__int128 f = prod; f <= LIMIT; f *= primes[k]) {
        rec(k + 1, f);
      }
    }
  }

  void run(int first_prime, int last_prime, u64* dest, int& size) {
    ptr = 0;
    last = last_prime;
    v = dest;
    rec(first_prime, 1);
    std::sort(dest, dest + ptr);
    size = ptr;
  }
};

ugly_generator ugly_gen;

void read_data() {
  scanf("%d", &num_primes);
  for (int i = 0; i < num_primes; i++) {
    scanf("%d", &primes[i]);
  }
  scanf("%llu", &kth);
  mid = num_primes * LEFT_FRAC / MAX_PRIMES;
}

u64 count_ugly(u64 up_to) {
  u64 result = 0;
  int j = num_ugly2 - 1;
  for (int i = 0; i < num_ugly1; i++) {
    while ((__int128)ugly1[i] * ugly2[j] > up_to) {
      j--;
    }
    result += (j + 1);
  }
  return result;
}

u64 bin_search_kth_number() {
  u64 l = 0, r = LIMIT; // (l exclusive, r inclusive]
  while (r - l > 1) {
    u64 mid = (l + r) / 2;
    if (count_ugly(mid) < kth) {
      l = mid;
    } else {
      r = mid;
    }
  }
  return r;
}

int main() {
  read_data();
  ugly_gen.run(0, mid, ugly1, num_ugly1);
  ugly_gen.run(mid, num_primes, ugly2, num_ugly2);
  u64 ans = bin_search_kth_number();
  printf("%llu\n", ans);

  return 0;
}
