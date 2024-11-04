// Reimplementation of https://codeforces.com/contest/687/submission/18789803
//
// Consider k = 100 = 2^2 × 5^2. If x mod k is uniquely determined (e.g. 57),
// then we can trivially find x mod 4 and x mod 25. The contrapositive is: if
// we cannot find both x mod 4 and x mod 25, then x mod k is not unique.
//
// To be able to find x mod 4, we need some c_i to be divisible by 4. If all
// the c's are either odd or only divisible by 2, we cannot uniquely determine
// x mod 4. If some of the c's are divisible by 8, 16 etc., then we can
// trivially recover x mod 4.
//
// The same goes for every other prime factor p of k. Let e be p's exponent in
// k. We wish the maximum exponent of p among all c's to be ≥ e. We can
// rephrase this as: take max(min(e, exponent of p in c_i)) for all i and see
// if the result is equal to e.
//
// This is precisely what gcd and lcm accomplish, for all prime factors
// simultaneously. Note that none of the intermediary values exceed k.
#include <stdio.h>

int gcd(int a, int b) {
  while (b) {
    int tmp = a;
    a = b;
    b = tmp % b;
  }
  return a;
}

int lcm(int a, int b) {
  return a / gcd(a, b) * b;
}

int main() {
  int n, k, c;
  int minimax = 1;

  scanf("%d %d", &n, &k);
  while (n--) {
    scanf("%d", &c);
    minimax = lcm(minimax, gcd(k, c));
  }

  printf((minimax == k) ? "Yes\n" : "No\n");

  return 0;
}
