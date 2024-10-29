// Let f(i,d) be the number of arrays using values in 1...i such that its GCD
// is a multiple of d. Then f(i,d) = ⌊i/d⌋^n. We can precompute all x^n values
// for later use. For a fixed i, we can use PIE to count the coprime arrays
// C(i) by iterating d from 1 to i:
//
// C(i) = f(i,1) - f(i,2) - f(i,3) - f(i,5) + f(i,2*3) ... - f(i,2*3*5) etc.
//
// We only care about square-free values of d, as is the case in many PIE
// problems, so we can use the Möbius function:
//
// C(i) = \sum_{d=1}^{i} μ(d) f(i,d)
//
// Thus, computing C(i) is O(k), so computing the answer is O(k^2). To improve
// this, note that f(i,d) only differs from f(i-1,d) if d|i. For example,
// ⌊13/5⌋^n = 2^n, ⌊14/5⌋^n = 2^n, but ⌊15/5⌋^n = 3^n. So we can maintain the
// running sum S(i) of all f(i,*). One approach is: for the current i, iterate
// through all of i's divisors and update S accordingly.
//
// A better approach is to precompute all the changes at the start. Then S(i)
// will be the partial sum of these changes up to i. To accomplish this, note
// that divisor 1 contributes to all the changes, divisor 2 contributes to
// every other change and so forth.
//
// Example:
//
// i \ d |   1     2     3    4     5     6     7    8    9    10    11
// ------+--------------------------------------------------------------------
//   13  | 13^n - 6^n - 4^n + XX - 2^n + 2^n - 1^n + XX + XX + 1^n - 1^n + ...
//   14  | 14^n - 7^n - 4^n + XX - 2^n + 2^n - 2^n + XX + XX + 1^n - 1^n + ...
//
// S(13) consists of 13^n arrays whose GCD is a multiple of 1, minus 6^n
// arrays with values from the set (2,4,6,8,10,12), whose GCD is a multiple of
// 2 etc. The lines for i=13 and i=14 differ at positions 1, 2, 7 and 14
// (which are the divisors of 14).
#include <stdio.h>

const int MAX_VAL = 2'000'000;
const int MOD = 1'000'000'007;

int mu[MAX_VAL + 1];
int pow_n[MAX_VAL + 1];
int changes[MAX_VAL + 1];
int n, k;

void read_data() {
  scanf("%d %d", &n, &k);
}

int bin_exp(int b, int e) {
  long long result = 1;

  while (e) {
    if (e & 1) {
      result = result * b % MOD;
    }
    b = (long long)b * b % MOD;
    e >>= 1;
  }

  return result;
}

void compute_powers() {
  for (int i = 1; i <= k; i++) {
    pow_n[i] = bin_exp(i, n);
  }
}

void compute_mobius(int n) {
  for (int i = 1; i <= n; i++) {
    mu[i] = 1;
  }

  for (int i = 2; i * i <= n; i++) {
    if (mu[i] == 1) {
      // This will replace each number with the signed product of its prime
      // factors, disregarding exponents.
      for (int j = i; j <= n; j += i) {
        mu[j] *= -i;
      }
      // Clear numbers which are non-square-free due to i.
      for (int j = i * i; j <= n; j += i * i) {
        mu[j] = 0;
      }
    }
  }

  for (int i = 2; i <= n; i++) {
    if (mu[i] == i) {
      mu[i] = 1;
    } else if (mu[i] == -i) {
      mu[i] = -1;
    } else if (mu[i] < 0) {
      // This case means that i has a prime factor larger than sqrt(n), which
      // was not counted.
      mu[i] = 1;
    } else if (mu[i] > 0) {
      mu[i] = -1;
    }
  }
}

void compute_changes() {
  for (int div = 1; div <= k; div++) {
    for (int quot = 1, mult = div; mult <= k; quot++, mult += div) {
      long long delta = pow_n[quot] - pow_n[quot - 1];
      delta = delta * mu[div] + MOD;
      changes[mult] = (changes[mult] + delta) % MOD;
    }
  }
}

void count_arrays() {
  int num_arrays = 0;
  int answer = 0;
  for (int i = 1; i <= k; i++) {
    num_arrays = (num_arrays + changes[i]) % MOD;
    answer = (answer + (num_arrays ^ i)) % MOD;
  }
  printf("%d\n", answer);
}

int main() {
  read_data();
  compute_powers();
  compute_mobius(k);
  compute_changes();
  count_arrays();

  return 0;
}
