// Method described in https://codeforces.com/blog/entry/101663.
//
// Select some pairwise coprime moduli such that their product exceeds
// 1'000'000'000 and the maximum modulus is minimized. Ask questions of the
// form (a, a + BIG_MOD) for all 1 ≤ a ≤ 23. If the gcd is a multiple of some
// modulus m, then x + a + BIG_MOD is a multiple of m, so x ≡ -a (mod m).
//
// After learning all the remainders, solve the Chinese Remainders Theorem.
#include <stdio.h>

// 8 pairwise coprime moduli and their product.
const int NUM_MODULI = 8;
const int MOD[NUM_MODULI] = { 16, 9, 7, 11, 13, 17, 19, 23 };
const int BIG_MOD = 1'070'845'776;

int coef[NUM_MODULI];
int rem[NUM_MODULI];

void extended_euclid_iterative(int a, int b, int& d, int& x, int& y) {
  x = 1;
  y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = x - q * xp; x = tmp;
    tmp = yp; yp = y - q * yp; y = tmp;
  }
  d = a;
}

// Use the extended Euclid's algorithm, not Fermat's algorithm because some
// moduli are composite.
int inverse(int x, int mod) {
  int y, k, d;
  extended_euclid_iterative(x, mod, d, y, k);
  return (y >= 0) ? y : (y + mod);
}

// Precompute the common part of the CRT (every modulus' coefficient).
void precompute() {
  for (int i = 0; i < NUM_MODULI; i++) {
    int prod_exclude = BIG_MOD / MOD[i];
    int inv = inverse(prod_exclude, MOD[i]);
    coef[i] = (long long)prod_exclude * inv % BIG_MOD;
  }
}

unsigned ask(unsigned a, unsigned b) {
  printf("? %u %u\n", a, b);
  fflush(stdout);

  int response;
  scanf("%d", &response);
  return response;
}

int solve_crt() {
  long long result = 0;
  for (int i = 0; i < NUM_MODULI; i++) {
    result += (long long)rem[i] * coef[i];
  }
  return result % BIG_MOD;
}

void solve_test_case() {
  for (int a = 23; a >= 1; a--) {
    int gcd = ask(a, a + BIG_MOD);
    for (int i = 0; i < NUM_MODULI; i++) {
      if (gcd % MOD[i] == 0) {
        // This can be negative for large values of a, but we'll find a
        // positive one later on.
        rem[i] = MOD[i] - a;
        // printf("Learned that x ≡ %d mod %d\n", rem[i], MOD[i]);
      }
    }
  }

  int solution = solve_crt();
  printf("! %u\n", solution);
}

int main() {
  precompute();

  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test_case();
  }

  return 0;
}
