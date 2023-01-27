#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define N 1'000'000
#define DECAY 1'000
#define SIGMA 6

typedef int vector[N + 1];

char s[N + 1];
vector p, c, p2, c2, cnt, lcp;

int circ(int x, int n) {
  return (x >= n) ? (x - n) : x;
}

void build_suffix_array() {
  int n = N + 1;

  memset(cnt, 0, sizeof(int) * 256);
  for (int i = 0; i < n; i++) {
    cnt[(int)s[i]]++;
  }
  for (int i = 1; i < 'a' + SIGMA; i++) {
    cnt[i] += cnt[i - 1];
  }

  for (int i = 0; i < n; i++) {
    p[--cnt[(int)s[i]]] = i;
  }

  int num_classes = 1;
  c[p[0]] = 0;
  for (int i = 1; i < n; i++) {
    if (s[p[i]] != s[p[i - 1]]) {
      num_classes++;
    }
    c[p[i]] = num_classes - 1;
  }

  int len = 1;
  while (num_classes < n) {
    memset(cnt, 0, sizeof(int) * num_classes);
    for (int i = 0; i < n; i++) {
      cnt[c[i]]++;
    }
    for (int i = 1; i < num_classes; i++) {
      cnt[i] += cnt[i - 1];
    }

    for (int i = 0; i < n; i++) {
      p2[i] = circ(p[i] - len + n, n);
    }
    for (int i = n - 1; i >= 0; i--) {
      p[--cnt[c[p2[i]]]] = p2[i];
    }

    num_classes = 1;
    c2[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      if (c[p[i]] != c[p[i - 1]] ||
          c[circ(p[i] + len, n)] != c[circ(p[i - 1] + len, n)]) {
        num_classes++;
      }
      c2[p[i]] = num_classes - 1;
    }
    memcpy(c, c2, sizeof(int) * n);
    len <<= 1;
  }
}

void build_lcp_array() {
  // Inversează-l pe p în p2
  for (int i = 0; i <= N; i++) {
    p2[p[i]] = i;
  }
  // Acum p2[i] ne spune al cîtelea sufix, în ordine lexicografică, este s[i...N]

  lcp[0] = 0; // Tehnic, nedefinit
  int l = 0;
  for (int i = 0; i <= N; i++) {
    int k = p2[i];
    if (k) {
      // Știm că LCP(p[k - 1], p[k]) >= l. Creștem naiv cît putem.
      int j = p[k - 1];
      while (s[i + l] == s[j + l]) {
        l++;
      }
      lcp[k] = l;

      // Pregătim l-ul pentru următorul k.
      if (l) {
        l--;
      }
    }
  }
}

int main() {

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  // Generează un șir de lungime N.
  for (int i = 0; i < N; i++) {
    int c = 0;
    while (c < SIGMA - 1 && rand() % 1'000'000 < DECAY) {
      c++;
    }
    s[i] = 'a' + c;
  }
  s[N] = '\0';

  build_suffix_array();
  build_lcp_array();

  for (int i = 1; i <= N; i++) {
    // Verificăm că exact lcp[i] octeți coincid.
    assert(strncmp(s + p[i - 1], s + p[i], lcp[i]) == 0);
    assert(strncmp(s + p[i - 1], s + p[i], lcp[i] + 1) < 0);
  }

  return 0;
}
