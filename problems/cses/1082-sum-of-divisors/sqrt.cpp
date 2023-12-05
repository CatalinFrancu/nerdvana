#include <math.h>
#include <stdio.h>

const int MOD = 1'000'000'007;
const int HALF = (1 + MOD) / 2;

int main() {
  long long n;
  scanf("%lld", &n);
  int root = sqrt(n);

  // Up to sqrt(n)
  long long result = 0;
  for (int d = 1; n / d > root; d++) {
    // printf("Take divisor %d %lld times\n", d, n / d);
    result = (result + n / d * d) % MOD;
  }

  // Beyond sqrt(n)
  for (int k = root; k >= 1; k--) {
    long long min_d = n / (k + 1) + 1;
    long long max_d = n / k;

    long long gauss = (min_d + max_d) % MOD *
      ((max_d - min_d + 1) % MOD) % MOD *
      HALF % MOD;

    result = (result + gauss * k) % MOD;
    // printf("Take divisors from %lld to %lld %d times\n", min_d, max_d, k);
  }

  printf("%lld\n", result);
  return 0;
}

// Perform some aggregate computations.
//
// Divisor k appears in k, 2k, 3k, ..., so ⌊n/k⌋ times. So the first iteration
// is: sum_{k=1}^{n} ⌊n/k⌋ * k. However, we must do better than O(n). So only
// compute the sum up to sqrt(n).
//
// Afterwards, ⌊n/k⌋ decreases slowly, so we ask the question: Which divisors
// should be counted k times? For example, if n = 100, then the divisors from
// 21 to 25 should be counted 4 times and the divisors from 26 to 33 should be
// counted 3 times. To find the answer, let d be the divisor and k = ⌊n/d⌋
// (divisor d should be counted k times).
//
// By definition, n/d - 1 < ⌊n/d⌋ ≤ n/d ==>
// n/d - 1 < k ≤ n/d ==>
// n - d < kd ≤ n ==>
// n/(k+1) < d ≤ n/k
