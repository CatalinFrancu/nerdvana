// Complexity: O(n log n) for computing Euler's totient function.
//
// Method: consider an nxn matrix, where only the half under the diagonal is
// defined, where A(i,j) is 1 if gcd(i,j) > 0 and 0 otherwise. Then by
// definition the total on row i is the number of numbers NOT coprime with i,
// namely i-φ(i). The answer for a given n is \sum_{i=1}^n (i - φ(i)).
#include <stdio.h>

const int MAX_N = 100'000;

int phi[MAX_N + 1];
long long answer[MAX_N + 1];

void compute_all_phi() {
  for (int i = 0; i <= MAX_N; i++)
    phi[i] = i;

  for (int i = 2; i <= MAX_N; i++) {
    if (phi[i] == i) {
      for (int j = i; j <= MAX_N; j += i) {
        phi[j] -= phi[j] / i;
      }
    }
  }
}

void compute_all_answers() {
  for (int i = 1; i <= MAX_N; i++) {
    answer[i] = answer[i - 1] + (i - phi[i]);
  }
}

void serve_answers() {
  int num_queries, n;
  scanf("%d", &num_queries);
  for (int q = 1; q <= num_queries; q++) {
    scanf("%d", &n);
    printf("Case %d: %lld\n", q, answer[n]);
  }
}

int main() {
  compute_all_phi();
  compute_all_answers();
  serve_answers();

  return 0;
}
