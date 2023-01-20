#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define TRIALS 10'000
#define N 10'000
#define M 500
#define SIGMA 4

int min(int x, int y) {
  return (x < y) ? x : y;
}

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

char random_char() {
  return rand() % SIGMA + 'a';
}

void naive(char* p, int m, char* s, int n, int* match, int* num_matches) {
  *num_matches = 0;

  // try every shift naively
  for (int i = 0; i <= n - m; i++) {
    int j = 0;
    while (j < M && p[j] == s[i + j]) {
      j++;
    }
    if (j == M) {
      match[(*num_matches)++] = i;
    }
  }
}

void kmp(char* p, int m, char* s, int n, int* pi, int* match, int* num_matches) {
  // Build the prefix function
  pi[0] = 0;
  int k = 0;
  for (int q = 1; q < m; q++) {
    while ((k > 0) && (p[k] != p[q])) {
      k = pi[k - 1];
    }
    if (p[k] == p[q]) {
      k++;
    }
    pi[q] = k;
  }

  // Run the search
  int q = 0; // current state
  *num_matches = 0;
  for (int i = 0; i < n; i++) {
    while ((q > 0) && (p[q] != s[i])) {
      q = pi[q - 1];
    }
    if (p[q] == s[i]) {
      q++;
    }
    if (q == m) {
      match[(*num_matches)++] = i - m + 1;
    }
  }
}

int main() {

  char p[M + 1], s[N + 1];
  int pi[M], match1[N], match2[N], num_matches1, num_matches2;

  init_rng();

  for (int t = 0; t < TRIALS; t++) {
    // build the arrays
    for (int i = 0; i < M; i++) {
      p[i] = random_char();
    }
    for (int i = 0; i < N; i++) {
      s[i] = random_char();
    }
    p[M] = s[N] = '\0';

    // ensure some repetitions of P within itself and within S
    int pos = rand() % M;
    memcpy(p + pos, p, M - pos);
    for (int i = 0; i < N / 2; i++) {
      int pos = rand() % N;
      memcpy(s + pos, p, min(N - pos, M));
    }

    naive(p, M, s, N, match1, &num_matches1);
    kmp(p, M, s, N, pi, match2, &num_matches2);

    // ensure identical results
    assert(num_matches1 == num_matches2);
    for (int i = 0; i < num_matches1; i++) {
      assert(match1[i] == match2[i]);
    }
  }

  return 0;
}
