// Complexity: O(n log n).
//
// Precompute φ, Euler's totient function. Now define an nxn matrix, where
// only the half above the diagonal is defined, where A(i,j) = gcd(i,j). Then
// the answer for any given n is the partial sum from (1,2) to (n,n).
//
// We won't compute the entire matrix, only column sums. Consider a GCD of
// g. On which columns will g appear and how many times? Obviously, g will
// only appear on columns whose index is a multiple of g. Furthermore, g will
// appear on column kg on those rows that are also multiples of g, but share
// no other common factors with k. By definition, the number of those rows is
// φ(k).
//
// To sum up, the contribution of every GCD g on column kg is g · φ(k).
#include <stdio.h>

const int MAX_N = 1'000'000;

int phi[MAX_N + 1];
long long answer[MAX_N + 1];

void compute_all_phi() {
  for (int i = 0; i <= MAX_N; i++) {
    phi[i] = i;
  }

  for (int i = 2; i <= MAX_N; i++) {
    if (phi[i] == i) {
      for (int j = i; j <= MAX_N; j += i) {
        phi[j] -= phi[j] / i;
      }
    }
  }
}

void compute_all_answers() {
  for (int gcd = 1; gcd <= MAX_N; gcd++) {
    for (int k = 2; k * gcd <= MAX_N; k++) {
      answer[k * gcd] += gcd * phi[k];
    }
  }

  for (int col = 2; col <= MAX_N; col++) {
    answer[col] += answer[col - 1];
  }
}

void serve_answers() {
  int n;
  while (scanf("%d", &n) && n) {
    printf("%lld\n", answer[n]);
  }
}

int main() {
  compute_all_phi();
  compute_all_answers();
  serve_answers();

  return 0;
}
