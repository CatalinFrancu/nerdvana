// Given a and n, computes 1 + a + a^2 + ... + a^{n-1} modulo MOD.
#include <assert.h>
#include <stdio.h>

const int MAX_A = 100;
const int MAX_N = 100;
const int MOD = 1'000'000'007;

int prog_sum_naive(int a, int n) {
  long long sum = 0;
  long long factor = 1;
  for (int p = 0; p < n; p++) {
    sum += factor;
    factor = factor * a % MOD;
  }
  sum %= MOD;
  return sum;
}

int prog_sum_iterative(int a, int n) {
  int num_bits = 32 - __builtin_clz(n);
  long long sum = 0;
  long long ak = 1;

  for (int i = 0; i < num_bits; i++) {
    // Contract: let k be the number consisting of the first i bits of n.
    // Then at this point ak = a^k and sum = 1 + a + ... + a^{k-1}.
    // Use the old value of ak (that is, a^{k/2}) to compute the new sum.
    sum = (1 + ak) * sum % MOD;
    ak = ak * ak % MOD;

    int bit = (n >> (num_bits - i - 1)) & 1;
    if (bit) {
      sum = (1 + a * sum) % MOD;
      ak = ak * a % MOD;
    }
  }

  return sum;
}

// Same as prog_sum_iterative(), but reverse the direction of i for slightly
// shorter code.
int prog_sum_iterative_2(int a, int n) {
  int num_bits = 32 - __builtin_clz(n);
  long long sum = 0;
  long long ak = 1;

  for (int i = num_bits - 1; i >= 0; i--) {
    sum = (1 + ak) * sum % MOD;
    ak = ak * ak % MOD;

    if ((n >> i) & 1) {
      sum = (1 + a * sum) % MOD;
      ak = ak * a % MOD;
    }
  }

  return sum;
}

struct prog {
  long long term; // stores a^n for some n
  long long sum;  // stores 1 + a + ... + a^{n-1} for the same n
};

prog prog_sum_recursive(int a, int n) {
  if (n == 1) {
    return { .term = a, .sum = 1 };
  }

  if (n & 1) {
    prog p = prog_sum_recursive(a, n - 1);
    return {
      p.term * a % MOD,
      (1 + a * p.sum) % MOD,
    };
  } else {
    prog p = prog_sum_recursive(a, n / 2);
    return {
      p.term * p.term % MOD,
      (1 + p.term) * p.sum % MOD,
    };
  }
}

int main() {
  for (int a = 1; a <= MAX_A; a++) {
    for (int n = 1; n <= MAX_N; n++) {
      int naive = prog_sum_naive(a, n);
      assert(naive == prog_sum_iterative(a, n));
      assert(naive == prog_sum_iterative_2(a, n));
      assert(naive == prog_sum_recursive(a, n).sum);
    }
  }

  return 0;
}
